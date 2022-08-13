#include "../include/client.hpp"

#include <iostream>

#include <boost/asio.hpp>

Client::Client(
  boost::asio::io_context& io_context, 
  std::string& server_ip,
  std::string& server_port
) : 
tcp_socket_(io_context),
tcp_resolver_(io_context) {
  boost::asio::connect(
    tcp_socket_, 
    tcp_resolver_.resolve(server_ip, server_port)
  );
}