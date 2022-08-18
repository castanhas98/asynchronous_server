#ifndef ASYNCHRONOUS_SERVER_SESSION_HPP
#define ASYNCHRONOUS_SERVER_SESSION_HPP

#include "chat.hpp"

#include <array>
#include <deque>
#include <memory>

#include <boost/asio.hpp>

class Session : public ChatParticipant, public std::enable_shared_from_this<Session> {
public:
  Session(boost::asio::ip::tcp::socket tcp_socket, ChatRoom& room);

  const boost::asio::ip::tcp::socket& get_tcp_socket() const;
  boost::asio::ip::tcp::socket& get_tcp_socket();

  void start();

  boost::asio::ip::address get_endpoint_ip_address();

  boost::asio::ip::port_type get_endpoint_port();

  void deliver(const ChatMessage& msg);

private:
  void do_read();

  void do_write(std::size_t length);

  void do_write();

  void do_read_header();

  void do_read_body();

  // prints non '\0' data characters.
  void print_data();

private:
  // The socket used to communicate with a TCP client
  boost::asio::ip::tcp::socket tcp_socket_;


  // The socket used to communicate with a UDP client
  // boost::asio::ip::udp::socket udp_socket_;

  // Buffer used to store data received from the client.
  std::array<char, 1024> data_;

  ChatMessage read_msg_;
  std::deque<ChatMessage> write_msgs_;

  ChatRoom room_;
};




#endif // ASYNCHRONOUS_SERVER_SESSION_HPP