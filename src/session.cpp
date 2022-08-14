#include "session.hpp"

#include <iostream>

#include <boost/asio.hpp>

Session::Session(boost::asio::ip::tcp::socket tcp_socket) 
: tcp_socket_(std::move(tcp_socket)) {
  std::cout << "Connection established with " 
            << tcp_socket_.remote_endpoint().address() 
            << " on port " 
            << tcp_socket_.remote_endpoint().port() << "." << std::endl;
}

// boost::asio::ip::tcp::socket& Session::get_tcp_socket() {
//   return tcp_socket_;
// }

void Session::start() {
  do_read();
}

void Session::do_read() {
  auto self(shared_from_this());
  
  tcp_socket_.async_read_some(
    boost::asio::buffer(data_),
    [this, self](boost::system::error_code ec, std::size_t length) {
      if(!ec) {
        print_data();
        // std::cout << "TEST" << std::endl;

        // std::cout << "JUST WROTE" << std::endl;
        // for(const auto &it: data_)
        //   std::cout << it << ", ";
        // std::cout << "\n";
        do_write(length);

        // Resets data after every read/write cycle
        data_.fill('\0');
      }
    }
  );

}

void Session::do_write(std::size_t length) {
  auto self(shared_from_this());

  boost::asio::async_write(
    tcp_socket_,
    boost::asio::buffer(data_),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
      if(!ec) {
        do_read();
      }
    }
  );
}


void Session::print_data() {
  for(int i = 0; i < data_.size() && data_[i] != '\0'; ++i)
    std::cout << data_[i];
  std::cout << std::endl;
}

boost::asio::ip::address Session::get_endpoint_ip_address() {
  return tcp_socket_.remote_endpoint().address();
}

boost::asio::ip::port_type Session::get_endpoint_port() {
  return tcp_socket_.remote_endpoint().port();
}