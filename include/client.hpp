#ifndef ASYNCHRONOUS_SERVER_CLIENT_HPP
#define ASYNCHRONOUS_SERVER_CLIENT_HPP

#include <string>

#include <boost/asio.hpp>

class Client {
  public:
    Client(
      boost::asio::io_context& io_context, 
      std::string& server_ip,
      std::string& server_port
    );

  private:
    boost::asio::ip::tcp::socket tcp_socket_;

    boost::asio::ip::tcp::resolver tcp_resolver_;
};

#endif // ASYNCHRONOUS_SERVER_CLIENT_HPP