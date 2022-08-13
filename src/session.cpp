#include "session.hpp"

Session::Session(boost::asio::ip::tcp::socket tcp_socket) 
: tcp_socket_(std::move(tcp_socket)) {
  
}

boost::asio::ip::tcp::socket& Session::get_tcp_socket() {
  return tcp_socket_;
}

void Session::start() {
  do_read();
}

void Session::do_read() {

}
