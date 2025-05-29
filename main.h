#pragma once

#include "volunteers.h"
#include "client.h"

struct Assignment
{
  Volunteer volunteer;
  client_ptr client;
  std::thread thread;
  std::thread heartbeat_thread;
  bool heartbeat_running;
  int heartbeat_interval;
};


typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef std::shared_ptr<client> client_ptr;
typedef std::shared_ptr<Assignment> asmt_ptr;