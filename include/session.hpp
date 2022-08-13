#ifndef ASYNCHRONOUS_SERVER_SESSION_HPP
#define ASYNCHRONOUS_SERVER_SESSION_HPP

#include <array>
#include <memory>

#include <boost/asio.hpp>

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(boost::asio::ip::tcp::socket tcp_socket);

  // boost::asio::ip::tcp::socket& get_tcp_socket();

  void start();

private:
  void do_read();

  void do_write(std::size_t length);

  // prints non '\0' data characters.
  void print_data();

private:
  // The socket used to communicate with a TCP client
  boost::asio::ip::tcp::socket tcp_socket_;


  // The socket used to communicate with a UDP client
  // boost::asio::ip::tcp::socket ip_socket_;

  // Buffer used to store data received from the client.
  std::array<char, 1024> data_;


};

#endif // ASYNCHRONOUS_SERVER_SESSION_HPP