#include "chat.hpp"
#include "client.hpp"

#include <array>
#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>

Client::Client(
  boost::asio::io_context& io_context, 
  std::string& server_ip,
  std::string& server_port
) : 
io_context_(io_context),
tcp_socket_(io_context),
tcp_resolver_(io_context) {
  // boost::asio::connect(
  //   tcp_socket_, 
  //   tcp_resolver_.resolve(server_ip, server_port)
  // );
  do_connect(server_ip, server_port);

  //
  io_context_thread_ = std::thread([&io_context](){ io_context.run(); });

  // do_echo();
  do_get_messages();
}

Client::~Client() {
  io_context_thread_.join();
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

void Client::do_connect(std::string& server_ip, std::string& server_port) {
  boost::asio::async_connect(
    tcp_socket_,
    tcp_resolver_.resolve(server_ip, server_port),
    [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
      if(!ec)
        do_read_header();
    }
  );
}

void Client::do_read_header() {
  std::cout << "In do read header:" << std::endl;
  boost::asio::async_read(
  tcp_socket_,
    boost::asio::buffer(read_msg_.data(), ChatMessage::header_length_),
    [this](boost::system::error_code ec, std::size_t /*length*/)
    {
      std::cout << "Handler do read header:" << std::endl;
      if(!ec && read_msg_.decode_header()) {
        do_read_body();
      }
      else
        tcp_socket_.close();
    }
  );
}

void Client::do_read_body() {
  std::cout << "In do read body:" << std::endl;
  boost::asio::async_read(
    tcp_socket_,
    boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
    [this](boost::system::error_code ec, std::size_t) {
      std::cout << "Handler do read body:" << std::endl;
      if (!ec) {
        std::cout.write(read_msg_.body(), read_msg_.body_length());
        std::cout << "\n";
        do_read_header();
      }
      else
        tcp_socket_.close();
    }
  );
} 

void Client::do_get_messages() {
  char line[ChatMessage::max_body_length_ + 1];
  while(std::cin.getline(line, ChatMessage::max_body_length_ + 1)) {

    ChatMessage msg;
    msg.body_length(std::strlen(line));
    std::memcpy(msg.body(), line, msg.body_length());
    msg.encode_header();
    write(msg);
  }
}

void Client::write(const ChatMessage& msg) {
  boost::asio::post(
    io_context_,
    [this, msg]() {
      bool write_in_progress = !write_msgs_.empty();
      write_msgs_.push_back(msg);
      if(!write_in_progress) {
        do_write();
      }

    }
  );
}

void Client::do_write() {
  std::cout << "Testing" << std::endl;
  boost::asio::async_write(
    tcp_socket_,
    boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
    [this](boost::system::error_code ec, std::size_t) {
      if(!ec) {
        write_msgs_.pop_front();
        if(!write_msgs_.empty())
          do_write();

      }

      else
        tcp_socket_.close();
    }
  );
}