cmake_minimum_required(VERSION 3.14)
project(vanity CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -mavx2")

include(FetchContent)

FetchContent_Declare(
  cpr 
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG 1.10.4
)

set(CPR_BUILD_TESTS OFF)
set(CPR_BUILD_SHARED_LIBS OFF) 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FetchContent_MakeAvailable(cpr)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

add_executable(sniper main.cpp)

target_include_directories(sniper PRIVATE ${cpr_SOURCE_DIR}/include)

target_link_libraries(sniper
  Boost::system
  OpenSSL::SSL
  OpenSSL::Crypto
  SQLite::SQLite3
  simdjson
  cpr::cpr
)