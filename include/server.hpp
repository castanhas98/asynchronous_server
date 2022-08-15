#ifndef ASYNCHRONOUS_SERVER_SERVER_HPP
#define ASYNCHRONOUS_SERVER_SERVER_HPP

#include "session.hpp"

#include <memory>
#include <set>
#include <thread>

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


  ~Server();

private:
  void do_accept();

  void print_active_sessions();

  void handle_console();

private:
  boost::asio::ip::tcp::acceptor tcp_acceptor_;
  // boost::asio::ip::udp::acceptor udp_acceptor_;

  // CHANGE FOR THREAD-SAFE
  std::set<std::shared_ptr<Session>> active_sessions_;

  std::thread console_thread_;

  bool server_online_;
};

#endif // ASYNCHRONOUS_SERVER_SERVER_HPP