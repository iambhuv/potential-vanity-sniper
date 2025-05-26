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

#include "patterns.h"

#include "config.cpp"
#include "volunteers.cpp"
#include "client.cpp"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef std::shared_ptr<client> client_ptr;

std::vector<Volunteer> volunteers;

struct Assignment
{
  Volunteer volunteer;
  client_ptr client;
  std::thread thread;
};

std::vector<Assignment> assignments;
Json::Value config;

int main()
{
  read_config_file(config);
  load_volunteers(config, volunteers);

  // std::cout << "Volunteers: " << std::endl;
  for (Volunteer &volunteer : volunteers)
  {
    init_client(config, volunteer);
    // std::cout << "ID: " << volunteer.id << "; Token: " << volunteer.token << std::endl;
  }
}