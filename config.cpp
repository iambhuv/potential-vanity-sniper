#include <simdjson.h>
#include <iostream>
#include <fstream>

#include "config.h"

const char *CONFIG_JSON_FILE = "snipe.json";


jelem read_config_file(simdjson::dom::parser &jparser)
{
  std::ifstream config_file(CONFIG_JSON_FILE);
  std::string config_json;

  if (config_file.is_open())
  {
    std::string line;
    while (std::getline(config_file, line))
    {
      config_json += line;
    }
    config_file.close();
  }
  else
  {
    std::cerr << "Cannot find snipe.json config file." << std::endl;
    throw std::runtime_error("Cannot find snipe.json config file.");
  }
  
  return jparser.parse(config_json).value();

  // for (auto vtr : config["volunteers"]) {
  //   std::cout << std::string_view(vtr) << std::endl;
  // }
}