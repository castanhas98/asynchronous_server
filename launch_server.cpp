#include "server.hpp"

#include <iostream>
#include <exception>

#include <thread>

#include <boost/asio.hpp>


using std::cout;
using std::endl;

void async_start_server(boost::asio::io_context& io_context, unsigned short &port) {
  Server server(io_context, port);
}

int main(int argc, char *argv[]) {

  unsigned short server_port;
  try {
    // can add more error checking for whether the porpt number is valid
    if(argc == 2)
      server_port = std::stoi(argv[1]);
    else if(argc > 2) {
      std::cerr << "Invalid amount of arguments. Intended usage: ./launch_server <server_port>" << std::endl;
      return 1;
    }

    boost::asio::io_context io_context;
    
    Server server(io_context, server_port);
    
    io_context.run();

  }
  catch(std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}