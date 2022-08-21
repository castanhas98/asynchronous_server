#include "chat.hpp"
#include "client.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>

Client::Client(
  boost::asio::io_context& io_context, 
  std::string& server_ip,
  std::string& server_port,
  std::string& user_name
) : 
io_context_(io_context),
tcp_socket_(io_context),
tcp_resolver_(io_context) {
  std::memcpy(user_name_, &user_name[0], user_name.size());

  do_connect(server_ip, server_port);

  // fill in the ip_and_port string with a padded ip and port
  std::size_t max_length_ip = 15;
  std::size_t max_length_port = 5;
  auto ip_and_port = "@" + std::string(
    max_length_ip - std::min(
      max_length_ip, tcp_socket_.local_endpoint().address().to_string().size()
    ), 
    ' '
  )
  + tcp_socket_.local_endpoint().address().to_string();
  ip_and_port = ip_and_port + ":" + std::to_string(tcp_socket_.local_endpoint().port()) +
  std::string(max_length_port - std::min(
    max_length_port, std::to_string(tcp_socket_.local_endpoint().port()).size()),
      ' ') + ": ";

  std::memcpy(ip_and_port_, &ip_and_port[0], ip_and_port.size());

  // thread to run the io_context after connecting
  io_context_thread_ = std::thread([&io_context](){ io_context.run(); });

  // console
  do_get_messages();
}

Client::~Client() {
  io_context_thread_.join();
}

void Client::do_connect(std::string& server_ip, std::string& server_port) {
  boost::asio::async_connect(
    tcp_socket_,
    tcp_resolver_.resolve(server_ip, server_port),
    [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint /*ep*/) {
      if(!ec)
        do_read_header();
    }
  );
}

void Client::do_read_header() {
  boost::asio::async_read(
  tcp_socket_,
    boost::asio::buffer(read_msg_.data(), ChatMessage::header_length_),
    [this](boost::system::error_code ec, std::size_t /*length*/) {
      if(!ec && read_msg_.decode_header()) {
        do_read_body();
      }
      else
        tcp_socket_.close();
    }
  );
}

void Client::do_read_body() {
  boost::asio::async_read(
    tcp_socket_,
    boost::asio::buffer(read_msg_.body_with_sender(), read_msg_.body_with_sender_length()),
    [this](boost::system::error_code ec, std::size_t /*length*/) {
      if (!ec) {
        std::cout.write(read_msg_.body_with_sender(), read_msg_.body_with_sender_length());
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
  while(tcp_socket_.is_open() && std::cin.getline(line, ChatMessage::max_body_length_ + 1)) {

    ChatMessage msg;
    msg.body_length(std::strlen(line));
    std::memcpy(msg.body(), line, msg.body_length());
    msg.encode_header();
    msg.encode_sender(user_name_, ip_and_port_);

    write(msg);

    if(std::string(msg.body(), msg.body() + 7) == "```exit")
      break;
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
  boost::asio::async_write(
    tcp_socket_,
    boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
    [this](boost::system::error_code ec, std::size_t /*length*/) {
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