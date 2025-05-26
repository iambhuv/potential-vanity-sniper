#pragma once

#include <iostream>
#include <string>

struct Volunteer
{
  std::string id;
  std::string token;
  std::string password;
  std::string payload;
};


template<typename... Args>
void vprint(Volunteer &volunteer, Args&&... args)
{
  std::cout << "[" << volunteer.id << "]: ";
  (std::cout << ... << args);
}

template<typename... Args>
void vprintl(Volunteer &volunteer, Args&&... args)
{
  std::cout << "[" << volunteer.id << "]: ";
  (std::cout << ... << args) << std::endl;
}