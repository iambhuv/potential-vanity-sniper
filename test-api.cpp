#include <iostream>
#include <simdjson.h>

// #include "api.cpp"
#include "totp.cpp"

#include <cpr/cpr.h>

const char *SECRET = "w7bb56nlgfdlhnfldgu26a6m6ccog6oa";

void test() {
  simdjson::dom::parser jparser;

  auto headers = cpr::Header{
      {"accept", "*/*"},
      {"accept-language", "en-US,en;q=0.9,hi;q=0.8"},
      {"authorization", " "},
      {"content-type", "application/json"},
      {"priority", "u=1, i"},
      {"sec-ch-ua", "\"Chromium\";v=\"136\", \"Google Chrome\";v=\"136\", "
                    "\"Not.A/Brand\";v=\"99\""},
      {"sec-ch-ua-mobile", "?0"},
      {"sec-ch-ua-platform", "\"Linux\""},
      {"sec-fetch-dest", "empty"},
      {"sec-fetch-mode", "cors"},
      {"sec-fetch-site", "same-origin"},
      {"x-context-properties", ""},
      {"x-debug-options", "bugReporterEnabled"},
      {"x-discord-locale", "en-US"},
      {"cookie", ""},
      {"x-discord-timezone", "Asia/Calcutta"},
      {"x-super-properties", ""}};

  auto res = cpr::Patch(cpr::Url{"https://canary.discord.com/api/v10/guilds/"
                                 "1374053641597616239/vanity-url"},
                        headers);

  auto rest = res.text;

  auto somshi = jparser.parse(rest).value();
  std::string ticket = std::string(std::string_view(somshi["mfa"]["ticket"]));

  std::cout << ticket << std::endl;

  std::string totp =
      std::to_string(generate_totp("w7bb56nlgfdlhnfldgu26a6m6ccog6oa"));

  std::string payload = "{\"ticket\":\"" + ticket +
                        "\",\"mfa_type\":\"totp\",\"data\":\"" + totp + "\"}";

  res = cpr::Post(cpr::Url{"https://canary.discord.com/api/v10/mfa/finish"},
                  headers, cpr::Body{payload});

  rest = res.text;

  std::cout << rest << std::endl;
}