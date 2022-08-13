#include "server.hpp"
#include "session.hpp"

#include <iostream>
#include <memory>

#include <boost/asio.hpp>

Server::Server(boost::asio::io_context& io_context, unsigned short port) 
: tcp_acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    std::cout << "Opening Server on port " << port << "." << std::endl;

    do_accept();
}

void Server::do_accept() {
  tcp_acceptor_.async_accept(
    [this](boost::system::error_code ec, boost::asio::ip::tcp::socket tcp_socket) {
      if(!ec) {
        std::make_shared<Session>(std::move(tcp_socket))->start();
      }

      do_accept();
    }
  );
}
