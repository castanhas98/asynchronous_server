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

  do_echo();
}

void Client::do_echo() {
  while(true) {
    message_.fill('\0');
    reply_.fill('\0');

    std::cout << "Enter message: ";
    std::cin.getline(&message_[0], message_.max_size());

    boost::asio::write(
      tcp_socket_, 
      boost::asio::buffer(message_, message_.size())
    );

    size_t reply_length = boost::asio::read(
      tcp_socket_,
      boost::asio::buffer(reply_, message_.size())
    );

    std::cout << "Reply is: ";
    std::cout.write(&reply_[0], reply_.size());
    std::cout << "\n";
  }
}