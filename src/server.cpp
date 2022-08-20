#include "server.hpp"
#include "session.hpp"
#include "threadsafe_data_structures.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include <chrono>

#include <boost/asio.hpp>

Server::Server(boost::asio::io_context& io_context, unsigned short port, unsigned short number_of_rooms) 
: tcp_acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
  server_online_(true),
  chat_rooms_(number_of_rooms) {
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
            std::make_shared<Session>(std::move(tcp_socket), room_);
          
          new_session->start();

          // active_sessions_.insert(new_session);
          active_sessions_.insert_or_update(
            new_session->get_endpoint_ip_address().to_string() + ":" + std::to_string(new_session->get_endpoint_port()),
            new_session
          );
          // print_active_sessions();
        }
        do_accept();
      }
    );
  }

}

void Server::print_active_sessions() {
  std::cout << "[" << std::this_thread::get_id() << "] ";
  std::map<std::string, std::shared_ptr<Session>> active_sessions_snapshot(
    active_sessions_.get_map()
  );
  
  if(active_sessions_snapshot.size() > 0) {
    std::cout << "Printing the IP addresses of clients in active sessions:" << std::endl;
    for(const auto &it : active_sessions_snapshot) {
      std::cout << "[" << std::this_thread::get_id() << "] ";
      std::cout << it.first << std::endl;

      if(it.second->get_tcp_socket().is_open())
        std::cout << "IS OPEN" << std::endl;  
    }
  }
  else
    std::cout << "There are no active sessions." << std::endl;
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

      //while !active_sessions_.empty()
      for(auto &it : active_sessions_.get_map())
        it.second->get_tcp_socket().close();
      
      // exit(0);
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