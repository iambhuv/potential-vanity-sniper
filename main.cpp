#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <jsoncpp/json/json.h>
#include <boost/asio/ssl/context.hpp>

#include "sbm_search.cpp"
#include "patterns.hpp"
#include "config.cpp"
#include "volunteers.cpp"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef std::shared_ptr<client> client_ptr;
std::vector<client_ptr> clients;

std::vector<Volunteer> volunteers;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;

std::shared_ptr<boost::asio::ssl::context> on_tls_init(const std::string &hostname)
{
  auto ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12_client);

  try
  {
    ctx->set_verify_mode(boost::asio::ssl::verify_peer);
    ctx->set_default_verify_paths();
  }
  catch (std::exception &e)
  {
    std::cout << "TLS init failed: " << e.what() << std::endl;
  }

  return ctx;
}

Json::Value config;
Json::Value json_root;

int heartbeat_interval;

std::thread heartbeat_thread;
std::atomic<bool> hb_ackrunning{true};

void open_handler(websocketpp::connection_hdl &hdl, client_ptr cptr)
{
  std::cout << "Connected!" << std::endl;
  std::string init_pl = Json::writeString(Json::StreamWriterBuilder(), config["init_payload"]);
  cptr->send(hdl, init_pl, websocketpp::frame::opcode::text);
}

void setup_interval(client::connection_ptr con, client_ptr cptr)
{
  websocketpp::connection_hdl hdl = con->get_handle();

  std::cout << "Setting up heartbeat..!" << std::endl;

  while (hb_ackrunning)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(heartbeat_interval));

    std::string heartbeat = R"({"op":1,"d":null})";
    try
    {
      std::cout << "Sending : " << heartbeat << std::endl;
      cptr->send(hdl, heartbeat, websocketpp::frame::opcode::text);
      std::cout << "Sent." << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cerr << "Heartbeat Failed : " << e.what() << std::endl;
      break;
    }
  }
}

void message_handler(websocketpp::connection_hdl &hdl, message_ptr &msg, client_ptr cptr)
{
  const char *data = msg->get_raw_payload().c_str();

  if (hnsearch(data, E_READY))
  {
    std::cout << "[READY]" << std::endl;
  }
  else if (hnsearch(data, E_GUILD_UPDATE))
  {
    std::cout << "[GUILD_UPDATE]" << std::endl;
  }
  else
  {
    if (reader.parse(data, json_root))
    {
      if (json_root["t"].isNull() && json_root["s"].isNull() && json_root["op"].asInt() == 10)
      {
        heartbeat_interval = json_root["d"]["heartbeat_interval"].asInt();
        client::connection_ptr con = cptr->get_con_from_hdl(hdl);
        heartbeat_thread = std::thread(setup_interval, con, cptr);
      }
      else
      {

        std::cout << "[" << json_root["t"].asString() << "]";

        std::string payload = Json::writeString(Json::StreamWriterBuilder(), json_root["d"]);
        std::cout << ": " << payload.substr(0, 128) << std::endl;
      }
    }
  }
}

void create_client(std::string uid) {}

int main()
{
  read_config_file(config);
  load_volunteers(config, volunteers);

  std::string uri = config["gateway"].asString();
  client_ptr cptr = std::make_shared<client>();

  try
  {
    cptr->set_access_channels(websocketpp::log::alevel::none);
    cptr->clear_access_channels(websocketpp::log::alevel::frame_payload);

    cptr->init_asio();
    cptr->set_tls_init_handler(bind(&on_tls_init, "gateway.discord.gg"));

    cptr->set_open_handler([&](websocketpp::connection_hdl hdl)
                           { open_handler(hdl, cptr); });

    cptr->set_message_handler([&](websocketpp::connection_hdl hdl, message_ptr msg)
                              { message_handler(hdl, msg, cptr); });

    websocketpp::lib::error_code ec;
    client::connection_ptr con = cptr->get_connection(uri, ec);
    if (ec)
    {
      std::cout << "Connection failed: " << ec.message() << std::endl;
      return 1;
    }

    cptr->connect(con);
    cptr->run();

    hb_ackrunning = false;
    if (heartbeat_thread.joinable())
      heartbeat_thread.join();
  }
  catch (std::exception &e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
  }
}