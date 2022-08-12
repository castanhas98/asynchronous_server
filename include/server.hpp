#ifndef ASYNCHRONOUS_SERVER_SERVER_HPP
#define ASYNCHRONOUS_SERVER_SERVER_HPP

#include <boost/asio.hpp>

class Server {

public:
  Server(boost::asio::io_service& io_service, unsigned short port);

private:
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor tcp_acceptor_;
  // boost::asio::ip::udp::acceptor udp_acceptor_;
};

#endif // ASYNCHRONOUS_SERVER_SERVER_HPP