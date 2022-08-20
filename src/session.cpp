#include "session.hpp"

#include <iostream>

#include <boost/asio.hpp>

Session::Session(boost::asio::ip::tcp::socket tcp_socket, ChatRoom& room)
: tcp_socket_(std::move(tcp_socket)),
  room_(room) {
  std::cout << "\n[" << std::this_thread::get_id() << "] ";
  std::cout << "Connection established with " 
            << tcp_socket_.remote_endpoint().address() 
            << " on port " 
            << tcp_socket_.remote_endpoint().port() << "." << std::endl;
  std::cout << ">>> ";
  std::flush(std::cout); // print the >>>

}

const boost::asio::ip::tcp::socket& Session::get_tcp_socket() const {
  return tcp_socket_;
}

boost::asio::ip::tcp::socket& Session::get_tcp_socket() {
  return tcp_socket_;
}

void Session::start() {
  room_.join(shared_from_this());
  do_read_header();
}


void Session::do_read_header()
{
  auto self(shared_from_this());
  boost::asio::async_read(
    tcp_socket_,
    boost::asio::buffer(read_msg_.data(), ChatMessage::header_length_),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
      if(!ec && read_msg_.decode_header()) {
        do_read_body();
      }
      else
      {
        room_.leave(shared_from_this());
      }
    }
  );
}

void Session::do_read_body()
{
  auto self(shared_from_this());
  boost::asio::async_read(
    tcp_socket_,
    boost::asio::buffer(read_msg_.body_with_sender(), read_msg_.body_with_sender_length()),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
      if(!ec) {
        room_.deliver(read_msg_, self);
        do_read_header();
      }
      else
      {
        room_.leave(shared_from_this());
      }
    }
  );
}

void Session::do_write() {
  auto self(shared_from_this());
  boost::asio::async_write(
    tcp_socket_,
    boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
      if (!ec)
      {
        write_msgs_.pop_front();
        if (!write_msgs_.empty()) {
          do_write();
        }
      }
      else
      {
        room_.leave(shared_from_this());
      }
    }
  );
}


void Session::deliver(const ChatMessage& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
      do_write();

  }


boost::asio::ip::address Session::get_endpoint_ip_address() {
  return tcp_socket_.remote_endpoint().address();
}

boost::asio::ip::port_type Session::get_endpoint_port() {
  return tcp_socket_.remote_endpoint().port();
}