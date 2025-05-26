#include <iostream>

#include "client.h"
#include "handler.cpp"
#include "volunteers.h"

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


void init_client(Json::Value &config, Volunteer &volunteer)
{
  client_ptr cptr = std::make_shared<client>();
  std::string uri = config["gateway"].asString();

  try
  {
    cptr->set_access_channels(websocketpp::log::alevel::none);
    cptr->clear_access_channels(websocketpp::log::alevel::frame_payload);

    cptr->init_asio();
    cptr->set_tls_init_handler(bind(&on_tls_init, "gateway.discord.gg"));

    cptr->set_open_handler([&](websocketpp::connection_hdl hdl)
                           { open_handler(hdl, cptr, volunteer); });

    cptr->set_message_handler([&](websocketpp::connection_hdl hdl, message_ptr msg)
                              { message_handler(hdl, msg, cptr, volunteer); });

    websocketpp::lib::error_code ec;
    client::connection_ptr con = cptr->get_connection(uri, ec);
    if (ec)
    {
      std::cout << "[" << volunteer.id << "] Connection failed: " << ec.message() << std::endl;
      
      throw std::runtime_error("Connection Failed");
    }

    cptr->connect(con);
    cptr->run();

    // hb_ackrunning = false;
    // if (heartbeat_thread.joinable())
    //   heartbeat_thread.join();
  }
  catch (std::exception &e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
  }
}