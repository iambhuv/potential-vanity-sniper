#include <functional>
#include <memory>
#include <vector>
#include <boost/asio/ssl/context.hpp>

#include "main.h"
#include "client.h"

#include "config.cpp"
#include "volunteers.cpp"
#include "client.cpp"

std::vector<Volunteer> volunteers;
std::vector<asmt_ptr> assignments;

int main()
{
  simdjson::dom::parser jparser;

  jelem config = read_config_file(jparser);
  
  load_volunteers(config, volunteers);

  for (Volunteer &volunteer : volunteers)
  {
    asmt_ptr assignment = std::make_shared<Assignment>();
    client_ptr cptr = std::make_shared<client>();
    assignment->volunteer = volunteer;
    assignment->client = cptr;
    assignment->heartbeat_running = true;
    assignment->thread = std::thread(init_client, std::ref(config), assignment);

    // init_client(config, assignment);

    assignments.push_back(assignment);
  }

  for (asmt_ptr a : assignments)
  {
    if (a->thread.joinable())
      a->thread.join();
  }
}