#include "server.hpp"
#include "session.hpp"

#include <iostream>
#include <memory>
#include <thread>

#include <chrono>

#include <boost/asio.hpp>

Server::Server(boost::asio::io_context& io_context, unsigned short port) 
: tcp_acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
  server_online_(true) {

    // Spawns a thread that will deal with console inputs on the server
    // This thread is joined in the Server's destructur, which should
    // only be called after the thread has no work left anyway.
    console_thread_ = std::thread(&Server::handle_console, this);

    // Current thread (the same that calls io_context::run()) now accepts
    // incoming connections
    do_accept();
}

void Server::do_accept() {
  // Accepts connections for as long as the server is online
  if(server_online_) {
    tcp_acceptor_.async_accept(
      [this](boost::system::error_code ec, boost::asio::ip::tcp::socket tcp_socket) {
        if(!ec) {
          std::shared_ptr<Session> new_session = 
            std::make_shared<Session>(std::move(tcp_socket));
          
          new_session->start();

          active_sessions_.insert(new_session);
          // print_active_sessions();
        }
        do_accept();
      }
    );
  }

}

void Server::print_active_sessions() {
  std::cout << "[" << std::this_thread::get_id() << "] ";
  std::cout << "Printing the IP addresses of clients in active sessions:" << std::endl;
  for(const auto &it: active_sessions_) {
    std::cout << "[" << std::this_thread::get_id() << "] ";
    std::cout << it->get_endpoint_ip_address() << ":"
              << it->get_endpoint_port() << std::endl;
  }
}

void Server::handle_console() {
  std::string action = "";

  // Indications and usage
  auto print_usage = []() {
    std::cout << "[" << std::this_thread::get_id() << "] ";
    std::cout << "Server Commands:" << std::endl;
    std::cout << "[" << std::this_thread::get_id() << "] ";
    std::cout << "active | exit | help \n" << std::endl;
  };

  print_usage();

  while(server_online_) {
    std::cout << "[" << std::this_thread::get_id() << "] ";
    std::cout << ">>> ";
    std::getline(std::cin, action);

    if(action == "active") {
      print_active_sessions();
      continue;
    }

    // On exit, set the state of the server to offline and close the
    // acceptor, so that more connections cannot be accepted further
    // and the existing ones are closed.
    else if(action == "exit") {
      tcp_acceptor_.close();
      server_online_ = false;
      break;
    }

    else if(action == "help") {
      std::cout << "[" << std::this_thread::get_id() << "] ";
      std::cout << "Asynchronous Server - Possible Commands:\n"
                << "active - prints the IP addresses of clients and ports to which they are connected;\n"
                << "exit - turns off the server.\n";
      print_usage();
      continue;
    }

    // no option was correct, print indications how to use help.
    else {
      continue;
    }
  }
}


Server::~Server() {
  console_thread_.join();
}