#include "client.h"
#include "config.h"
#include "main.h"
#include "patterns.h"
#include "sbm_search.cpp"
#include "volunteers.h"

void open_handler(websocketpp::connection_hdl &hdl, client_ptr cptr,
                  asmt_ptr asmt) {
  // std::cout << "[" << volunteer.id << "] Connected!" << std::endl;
  vprintl(asmt->volunteer, "Connected!");
  // vprintl(asmt->volunteer, "Payload: ", asmt->volunteer.payload);

  cptr->send(hdl, asmt->volunteer.payload, websocketpp::frame::opcode::text);
}

void setup_interval(client::connection_ptr con, client_ptr cptr,
                    asmt_ptr asmt) {
  websocketpp::connection_hdl hdl = con->get_handle();
  vprintl(asmt->volunteer, "Setting up heartbeat..!");

  while (asmt->heartbeat_running) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(asmt->heartbeat_interval));

    std::string heartbeat = R"({"op":1,"d":null})";
    try {
      vprintl(asmt->volunteer, "Sending HB!");
      cptr->send(hdl, heartbeat, websocketpp::frame::opcode::text);
      vprintl(asmt->volunteer, "Sent!");
    } catch (const std::exception &e) {
      vprintl(asmt->volunteer, "Heartbeat Failed : ", e.what());
      continue;
    }
  }
}

void message_handler(websocketpp::connection_hdl &hdl, message_ptr &msg,
                     client_ptr cptr, asmt_ptr asmt) {
  simdjson::dom::parser jparser;
  const char *data = msg->get_raw_payload().c_str();

  if (hnsearch(data, E_READY)) {
    vprintl(asmt->volunteer, "[READY]");
  } else if (hnsearch(data, E_GUILD_UPDATE)) {
    vprintl(asmt->volunteer, "[GUILD_UPDATE]");
  } else {
    jelem json_root = jparser.parse(std::string(data));

    if (json_root["t"].is_null() && json_root["s"].is_null() &&
        json_root["op"].is_int64() && json_root["op"].get_int64().value() == 10) {
      asmt->heartbeat_interval = json_root["d"]["heartbeat_interval"].get_int64().value();

      vprintl(asmt->volunteer,
              "Heartbeat Interval : ", asmt->heartbeat_interval);
      client::connection_ptr con = cptr->get_con_from_hdl(hdl);

      asmt->heartbeat_thread = std::thread(setup_interval, con, cptr, asmt);
    }
    // else {
    //   vprint(asmt->volunteer, "(", simdjson::to_string(json_root["t"]), ")");

    //   std::string payload = simdjson::to_string(json_root["d"]).substr(0,128);

    //   std::cout << ": " << payload << std::endl;
    // }
  }
}