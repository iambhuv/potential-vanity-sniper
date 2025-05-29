#pragma once

#include <iostream>
#include <string>
#include <websocketpp/sha1/sha1.hpp>

struct Volunteer {
  std::string id;
  std::string token;
  std::string password;
  std::string payload;
};

const int COL_RANGE = (231 - 16)+1;

inline unsigned int color_code(std::string &seed) {
  unsigned int sum = 0;

  for (int l = 0; l <= seed.length(); l++) {
    sum = sum + (seed[l] - '0');
  }
  return (sum % COL_RANGE) + 16;
}

template <typename... Args> void vprint(Volunteer &volunteer, Args &&...args) {
  std::cout << "\033[38;5;" << color_code(volunteer.id) << "m" << "[" << volunteer.id << "]: " << "\033[0m";
  (std::cout << ... << args);
}

template <typename... Args> void vprintl(Volunteer &volunteer, Args &&...args) {
  std::cout << "\033[38;5;" << color_code(volunteer.id) << "m" << "[" << volunteer.id << "]: "<< "\033[0m";
  (std::cout << ... << args) << std::endl;
}