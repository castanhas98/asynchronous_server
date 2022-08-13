#ifndef ASYNCHRONOUS_SERVER_SERVER_HPP
#define ASYNCHRONOUS_SERVER_SERVER_HPP

#include "session.hpp"

#include <memory>

#include <boost/asio.hpp>

class Server {

public:
  /**
   * @brief Constructs a new Server object. using io_server and the selected
   * port. Accepts clients connecting asynchronously.
   * 
   * @param io_service 
   * @param port 
   */
  Server(boost::asio::io_context& io_context, unsigned short port);

private:
  void do_accept();

private:
  boost::asio::ip::tcp::acceptor tcp_acceptor_;
  // boost::asio::ip::udp::acceptor udp_acceptor_;
};

#endif // ASYNCHRONOUS_SERVER_SERVER_HPP