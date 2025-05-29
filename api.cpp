// #include <iostream>
#include "config.h"
#include "main.h"
#include <curl/curl.h>
#include <string>

enum API_METHOD { GET, POST, PUT, DELETE };

enum HTTP_STATUS {
  BROKEN = 0,
  OK = 200,
  CREATED = 201,
  NO_CONTENT = 204,
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  INTERNAL_SERVER_ERROR = 500
};

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string *data) {
  data->append((char *)ptr, size * nmemb);
  return size * nmemb;
}

class CurlClient {
  CURL *curl;
  std::string base_url;
  struct curl_slist *headers = NULL;

public:
  CurlClient(const std::string &host = "https://canary.discord.com") : base_url(host) {
    curl = curl_easy_init();

    headers = curl_slist_append(headers, "accept: */*");
    headers = curl_slist_append(headers, "accept-language: en-US,en;q=0.9");
    headers = curl_slist_append(headers, "priority: u=1, i");
    headers = curl_slist_append(headers,
                                "sec-ch-ua: \"Chromium\";v=\"136\", \"Google "
                                "Chrome\";v=\"136\", \"Not.A/Brand\";v=\"99\"");
    headers = curl_slist_append(headers, "sec-ch-ua-mobile: ?0");
    headers = curl_slist_append(headers, "sec-ch-ua-platform: \"Linux\"");
    headers = curl_slist_append(headers, "sec-fetch-dest: empty");
    headers = curl_slist_append(headers, "sec-fetch-mode: cors");
    headers = curl_slist_append(headers, "sec-fetch-site: same-origin");
    headers = curl_slist_append(headers, "x-debug-options: bugReporterEnabled");
    headers = curl_slist_append(headers, "x-discord-locate: en-US");
    headers = curl_slist_append(headers, "x-discord-timezone: Asia/Calcutta");
    headers = curl_slist_append(
        headers,
        "x-super-properties: "
        "eyJvcyI6IkxpbnV4IiwiYnJvd3NlciI6IkNocm9tZSIsImRldmljZSI6IiIsInN5c3RlbV"
        "9sb2NhbGUiOiJlbi1VUyIsImhhc19jbGllbnRfbW9kcyI6ZmFsc2UsImJyb3dzZXJfdXNl"
        "cl9hZ2VudCI6Ik1vemlsbGEvNS4wIChYMTE7IExpbnV4IHg4Nl82NCkgQXBwbGVXZWJLaX"
        "QvNTM3LjM2IChLSFRNTCwgbGlrZSBHZWNrbykgQ2hyb21lLzEzNi4wLjAuMCBTYWZhcmkv"
        "NTM3LjM2IiwiYnJvd3Nlcl92ZXJzaW9uIjoiMTM2LjAuMC4wIiwib3NfdmVyc2lvbiI6Ii"
        "IsInJlZmVycmVyIjoiIiwicmVmZXJyaW5nX2RvbWFpbiI6IiIsInJlZmVycmVyX2N1cnJl"
        "bnQiOiIiLCJyZWZlcnJpbmdfZG9tYWluX2N1cnJlbnQiOiIiLCJyZWxlYXNlX2NoYW5uZW"
        "wiOiJzdGFibGUiLCJjbGllbnRfYnVpbGRfbnVtYmVyIjo0MDI0MDIsImNsaWVudF9ldmVu"
        "dF9zb3VyY2UiOm51bGwsImNsaWVudF9sYXVuY2hfaWQiOiI1MGMzM2IxNy03ZGU2LTQ4MT"
        "ItODExOC0zYzI0MzQ5NzEwMjkiLCJjbGllbnRfYXBwX3N0YXRlIjoiZm9jdXNlZCIsImNs"
        "aWVudF9oZWFydGJlYXRfc2Vzc2lvbl9pZCI6IjBiYTYxMDllLTljOTgtNDk5ZS05MDQ4LT"
        "Q2ODJjMTBjMTJjOSJ9");
    headers = curl_slist_append(headers, "content-type: application/json");

    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 0L);  // allow reuse
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L); // enable TCP keepalive
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,
                     "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) "
                     "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/136.0.0.0 "
                     "Mobile Safari/537.36");
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
  }

  ~CurlClient() { curl_easy_cleanup(curl); }

  void set_header(const std::string &header) {
    headers = curl_slist_append(headers, header.c_str());
  }

  std::string request(API_METHOD method, const std::string &path,
                      const std::string &body = "") {
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, (base_url + path).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    if (method == POST) {
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    } else if (method == PUT) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    } else if (method == DELETE) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    } else { // GET
      curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }

    curl_easy_perform(curl);
    return response;
  }
};

std::string api_get(asmt_ptr asmt, jelem &config, CurlClient &client,
                    std::string &path) {

  client.set_header(std::string("authorization: ") + asmt->volunteer.token);

  return client.request(GET, path);
}