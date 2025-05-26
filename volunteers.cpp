#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <jsoncpp/json/json.h>
#include "volunteers.h"

std::string base64_decode(const std::string_view in)
{
  // table from '+' to 'z'
  const uint8_t lookup[] = {
      62, 255, 62, 255, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255,
      255, 0, 255, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
      255, 255, 255, 255, 63, 255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
      36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
  static_assert(sizeof(lookup) == 'z' - '+' + 1);

  std::string out;
  int val = 0, valb = -8;
  for (uint8_t c : in)
  {
    if (c < '+' || c > 'z')
      break;
    c -= '+';
    if (lookup[c] >= 64)
      break;
    val = (val << 6) + lookup[c];
    valb += 6;
    if (valb >= 0)
    {
      out.push_back(char((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return out;
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter))
  {
    tokens.push_back(token);
  }
  return tokens;
}

std::string volunteer_payload(Json::Value &config, std::string &token)
{
  Json::Value init_pl = config["init_payload"];
  init_pl["d"]["token"] = token;
  return Json::writeString(Json::StreamWriterBuilder(), init_pl);
}

Volunteer load_volunteer(Json::Value &config, std::string &vstr)
{
  size_t length = sizeof(vstr);
  size_t passlen;
  Volunteer volunteer;

  try
  {
    std::vector<std::string> passntoken = split(vstr, ':');

    volunteer.password = passntoken[0];
    volunteer.token = passntoken[1];
    volunteer.id = base64_decode(split(passntoken[1], '.')[0]);
    volunteer.payload = volunteer_payload(config, volunteer.token);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  return volunteer;
}

void load_volunteers(Json::Value &config, std::vector<Volunteer> &volunteers)
{
  Json::Value volunt = config["volunteers"];

  if (!volunt.isArray())
  {
    throw std::runtime_error("Cannot find volunteers list in snipe.json.");
  }

  std::vector<std::string> volunteer_list;
  volunteer_list.reserve(volunt.size());

  std::transform(volunt.begin(), volunt.end(), std::back_inserter(volunteer_list), [](const Json::Value &e)
                 { return e.asString(); });

  for (std::string &volunteer : volunteer_list)
  {
    volunteers.push_back(load_volunteer(config, volunteer));
  }
}