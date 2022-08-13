#include "client.hpp"

#include <iostream>
#include <string>

#include <boost/asio.hpp>

int main(int argc, char *argv[]) {

  try {
    if(argc != 3) {
      std::cerr << "Invalid amount of arguments. Intended usage: ./launch_client <host_ip> <server_port>" << std::endl;
      return 1;
    }
    
    std::string host_ip(argv[1]);
    std::string server_port(argv[2]);

    std::cout << "Will attempt to connect to server with IPv4 address: "
              << host_ip << ":" << server_port << "." << std::endl;

    boost::asio::io_context io_context;

    Client client(io_context, host_ip, server_port);

  }
  catch(std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}