#include "session.hpp"

#include <iostream>

#include <boost/asio.hpp>

Session::Session(boost::asio::ip::tcp::socket tcp_socket, ChatRoom& room)
: tcp_socket_(std::move(tcp_socket)),
  room_(room) {
  std::cout << "[" << std::this_thread::get_id() << "] ";
  std::cout << "Connection established with " 
            << tcp_socket_.remote_endpoint().address() 
            << " on port " 
            << tcp_socket_.remote_endpoint().port() << "." << std::endl;
}

const boost::asio::ip::tcp::socket& Session::get_tcp_socket() const {
  return tcp_socket_;
}

boost::asio::ip::tcp::socket& Session::get_tcp_socket() {
  return tcp_socket_;
}

void Session::start() {
  // do_read();
  room_.join(shared_from_this());
  do_read_header();
}

void Session::do_read() {
  auto self(shared_from_this());
  
  tcp_socket_.async_read_some(
    boost::asio::buffer(data_),
    [this, self](boost::system::error_code ec, std::size_t length) {
      if(!ec) {
        print_data();

        do_write(length);

        // Resets data after every read/write cycle
        data_.fill('\0');
      }
    }
  );

}

// void Session::do_read() {
//   auto self(shared_from_this());
  
//   // boost::asio::async_read(
//   //   tcp_socket_,
//   //   boost::asio::buffer(read_msg_.data(), read_msg_.body_length()),
//   //   [this, self](boost::system::error_code ec, std::size_t) {
//   //     std::cout << "Testttt" << std::endl;
//   //     std::cout << "msg size = " << read_msg_.body_length() << std::endl;

//   //     if(!ec) {
//   //       std::cout << "[" << std::this_thread::get_id() << "] ";
//   //       for(int i = 0; i < read_msg_.body_length() && data_[i] != '\0'; ++i)
//   //         std::cout << read_msg_.data()[i];
//   //       std::cout << std::endl;

//   //       do_read();
//   //       // read_msg_.fill('\0');
//   //     }
//   //     // else {}
//   //   }
//   // );

//   boost::asio::async_read(tcp_socket_,
//     boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
//     [this, self](boost::system::error_code ec, std::size_t /*length*/)
//     {
//       if (!ec)
//       {
//         std::cout.write(read_msg_.body(), read_msg_.body_length());
//         do_read();
//       }
//       else
//       {
//         std::cerr << "ERROR" << std::endl;
//       }
//     });

//     std::cout.write(read_msg_.body(), read_msg_.body_length());
  

//   // tcp_socket_.async_read_some(
//   //   boost::asio::buffer(data_),
//   //   // [this, self](boost::system::error_code ec, std::size_t length) {
//   //   //   if(!ec) {
//   //   //     print_data();

//   //   //     do_write(length);

//   //   //     // Resets data after every read/write cycle
//   //   //     data_.fill('\0');
//   //   //   }
//   //   // }
//   //   [this, self](boost::system::error_code ec, std::size_t) {
//   //     if(!ec) {
//   //       // std::cout << "[" << std::this_thread::get_id() << "] ";
//   //       // for(int i = 0; i < read_msg_.body_length() && data_[i] != '\0'; ++i)
//   //       //   std::cout << read_msg_.data()[i];
//   //       // std::cout << std::endl;
//   //       std::cout << "test read" << std::endl;
//   //       print_data();

//   //       // Resets data after every read/write cycle
//   //       data_.fill('\0');

//   //       do_read();
//   //       // read_msg_.fill('\0');
//   //     }
//   //     // else {}
//   //   }
//   // );

// }

void Session::do_read_header()
{
  auto self(shared_from_this());
  boost::asio::async_read(
    tcp_socket_,
    boost::asio::buffer(read_msg_.data(), ChatMessage::header_length_),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
      std::cout << "We read a header!" << std::endl;
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
    boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
      std::cout << "We read a body!" << std::endl;
      if(!ec) {
        room_.deliver(read_msg_);
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
      std::cout << "We did a write!" << std::endl;
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

void Session::deliver(const ChatMessage& msg)
  {
    std::cout << "We are delivering." << std::endl;
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

void Session::print_data() {
  std::cout << "[" << std::this_thread::get_id() << "] ";
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