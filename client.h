#pragma once

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef std::shared_ptr<client> client_ptr;

typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;