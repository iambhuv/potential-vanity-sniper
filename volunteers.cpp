#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <jsoncpp/json/json.h>
#include <openssl/evp.h>

std::vector<unsigned char> base64_decode(const std::string &input)
{
  int len = input.size();
  std::vector<unsigned char> out((len * 3) / 4);
  int out_len = EVP_DecodeBlock(out.data(),
                                reinterpret_cast<const unsigned char *>(input.c_str()),
                                len);
  while (out_len > 0 && out[out_len - 1] == 0)
    out_len--;
  out.resize(out_len);
  return out;
}

struct Volunteer
{
  std::string id;
  std::string token;
  std::string password;
};

Volunteer load_volunteer(std::string &vstr)
{
  size_t length = sizeof(vstr);
  size_t passlen;
  Volunteer volunteer;

  for (size_t i = 0; i < length; i++)
  {
    if (vstr.at(i) == ':')
      passlen = i;
    if (vstr.at(i) == '.')
    {
      std::string id_b64 = vstr.substr(passlen + 1, i - (passlen + 1));
      std::vector<unsigned char> bid = base64_decode(id_b64);
      volunteer.id = std::string(bid.begin(), bid.end());
      break;
    }
  }
  volunteer.password = vstr.substr(0, passlen);
  volunteer.token = vstr.substr(passlen+1, length);

  return volunteer;
}

void load_volunteers(Json::Value &config, std::vector<Volunteer> volunteers)
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

  std::cout << "Volunteers: " << std::endl;
  for (std::string &volunteer : volunteer_list)
  {
    volunteers.push_back(load_volunteer(volunteer));
    std::cout << volunteer << std::endl;
  }
}