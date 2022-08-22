#include "session.hpp"

#include <iostream>

#include <boost/asio.hpp>

Session::Session(boost::asio::ip::tcp::socket tcp_socket, ChatRoom& room)
: tcp_socket_(std::move(tcp_socket)),
  room_(room) {
  // std::cout << "\n[" << std::this_thread::get_id() << "] ";
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


boost::asio::ip::address Session::get_endpoint_ip_address() {
  return tcp_socket_.remote_endpoint().address();
}

boost::asio::ip::port_type Session::get_endpoint_port() {
  return tcp_socket_.remote_endpoint().port();
}

void Session::deliver(const ChatMessage& msg) {
  bool write_in_progress = !write_msgs_.empty();
  write_msgs_.push_back(msg);
  if (!write_in_progress)
    do_write();
}

void Session::do_write() {
  auto self(shared_from_this());
  boost::asio::async_write(
    tcp_socket_,
    boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
      if (!ec)
      {
        // std::cout << "inside dowrite" << std::endl;
        // std::cout.write(write_msgs_.front().body_with_sender(), write_msgs_.front().body_with_sender_length());
        // std::cout << "\n\n" << std::flush;
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


void Session::do_read_header() {
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

void Session::do_read_body() {
  auto self(shared_from_this());
  boost::asio::async_read(
    tcp_socket_,
    boost::asio::buffer(read_msg_.body_with_sender(), read_msg_.body_with_sender_length()),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
      if(!ec) {
        find_command_and_send();
        do_read_header();
      }
      else
      {
        room_.leave(shared_from_this());
      }
    }
  );
}

void Session::find_command_and_send() {
  char *body = read_msg_.body();

  if(body[0] == '`' && body[2] == '`' && body[2] == '`') {
    if(std::string(body + 3, body + 7) == "exit") {
      room_.leave(shared_from_this());
      tcp_socket_.close();
      return;
    }
    else if(std::string(body + 3, body + 5) == "to") {
      
      int start_ip = 6;
      int i = 0;
      bool reached_port = false;
      int start_port, end_ip, end_port = start_ip + 14 + 6;

      while(i < read_msg_.body_length() && i < end_port) {
        if(body[start_ip + i] == ':') {
          reached_port = true;
          end_ip = start_ip + i;
          start_port = start_ip + i + 1;
        }

        if(reached_port && body[start_ip + i] == ' ' || body[start_ip + i] == '\0') {
          end_port = start_ip + i;
          break;
        }

        ++i;
      }

      std::string target_ip(body + start_ip, body + end_ip);
      std::string target_port(body + start_port, body + end_port);
      // std::cout << "length of port" << target_port.size() << std::endl;
      // target_port.push_back('\0');
      // std::cout << "length of port" << target_port.size() << std::endl;

      std::cout << target_ip << ":" << target_port << std::endl;

      // process message first
      // char* k = body;
      // char* j = body + end_port;

      // do {
      //   *k = *j;
      //   ++j;
      //   ++k;
      // } while(*j != '\0');

      // read_msg_.body_length(read_msg_.body_length() - end_port);

      // std::memmove(body, body + end_port, read_msg_.body_length() - end_port);
      // std::cout.write(write_msgs_.front().body_with_sender(), write_msgs_.front().body_with_sender_length());
      

      // std::cout.write(read_msg_.body(), read_msg_.body_length());
      // std::cout << '\n' << std::flush;

      // std::cout.write(read_msg_.body_with_sender(), read_msg_.body_with_sender_length());
      // std::cout << '\n' << std::flush;

      // std::cout << "message length:" << read_msg_.body_length();

      room_.deliver_to(read_msg_, shared_from_this(), target_ip, target_port);

      return;
    }
  }

  room_.deliver(read_msg_, shared_from_this());

  
}





