#ifndef ASYNCHRONOUS_SERVER_SESSION_HPP
#define ASYNCHRONOUS_SERVER_SESSION_HPP

#include <boost/asio.hpp>

class Session {
public:
  Session(boost::asio::ip::tcp::socket tcp_socket);

  boost::asio::ip::tcp::socket& get_tcp_socket();

  void start();

private:
  void do_read();

private:
  // The socket used to communicate with a TCP client
  boost::asio::ip::tcp::socket tcp_socket_;


  // The socket used to communicate with a UDP client
  // boost::asio::ip::tcp::socket ip_socket_;


};

#endif // ASYNCHRONOUS_SERVER_SESSION_HPP