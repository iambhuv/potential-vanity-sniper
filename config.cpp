#include <jsoncpp/json/json.h>
#include <iostream>
#include <fstream>

#include "config.h"

const char *CONFIG_JSON_FILE = "snipe.json";

void read_config_file(Json::Value &config)
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

  if (!reader.parse(config_json, config))
  {
    std::cerr << "Failed to parse config file!\n"
              << reader.getFormattedErrorMessages() << std::endl;
    throw std::runtime_error("Failed to parse config file!");
  }
}