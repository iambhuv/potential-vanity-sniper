#include <iostream>
#include <jsoncpp/json/json.h>
#include "client.h"
#include "patterns.h"
#include "sbm_search.cpp"
#include "config.h"
#include "volunteers.h"

Json::Value json_root;

void open_handler(websocketpp::connection_hdl &hdl, client_ptr cptr, Volunteer &volunteer)
{
  // std::cout << "[" << volunteer.id << "] Connected!" << std::endl;
  vprintl(volunteer, "Connected!");
  std::string init_pl = Json::writeString(Json::StreamWriterBuilder(), volunteer.payload);
  cptr->send(hdl, init_pl, websocketpp::frame::opcode::text);
}

void setup_interval(client::connection_ptr con, client_ptr cptr, Volunteer &volunteer)
{
  websocketpp::connection_hdl hdl = con->get_handle();

  vprintl(volunteer, "Setting up heartbeat..!");
  // std::cout << "Setting up heartbeat..!" << std::endl;

  // while (hb_ackrunning)
  // {
  //   std::this_thread::sleep_for(std::chrono::milliseconds(heartbeat_interval));

  //   std::string heartbeat = R"({"op":1,"d":null})";
  //   try
  //   {
  //     std::cout << "Sending : " << heartbeat << std::endl;
  //     cptr->send(hdl, heartbeat, websocketpp::frame::opcode::text);
  //     std::cout << "Sent." << std::endl;
  //   }
  //   catch (const std::exception &e)
  //   {
  //     std::cerr << "Heartbeat Failed : " << e.what() << std::endl;
  //     break;
  //   }
  // }
}

void message_handler(websocketpp::connection_hdl &hdl, message_ptr &msg, client_ptr cptr, Volunteer &volunteer)
{
  const char *data = msg->get_raw_payload().c_str();

  if (hnsearch(data, E_READY))
  {
    vprintl(volunteer, "[READY]");
  }
  else if (hnsearch(data, E_GUILD_UPDATE))
  {
    vprintl(volunteer, "[GUILD_UPDATE]");
  }
  else
  {
    if (reader.parse(data, json_root))
    {
      if (json_root["t"].isNull() && json_root["s"].isNull() && json_root["op"].asInt() == 10)
      {
        int heartbeat_interval = json_root["d"]["heartbeat_interval"].asInt();
        
        vprintl(volunteer, "Heartbeat Interval : ", heartbeat_interval);
        // std::cout < "Heartbeat Interval : " << heartbeat_interval << std::endl;

        // client::connection_ptr con = cptr->get_con_from_hdl(hdl);
        // heartbeat_thread = std::thread(setup_interval, con, cptr);
      }
      else
      {

        // std::cout << "[" << json_root["t"].asString() << "]";
        vprint(volunteer, "(", json_root["t"].asString(), ")");

        std::string payload = Json::writeString(Json::StreamWriterBuilder(), json_root["d"]);
        std::cout << ": " << payload.substr(0, 128) << std::endl;
      }
    }
  }
}