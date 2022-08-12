#include "server.hpp"

#include <iostream>

Server::Server(boost::asio::io_service& io_service, unsigned short port) 
: io_service_(io_service),
  tcp_acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {

}