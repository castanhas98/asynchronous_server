#include "client.hpp"

#include <iostream>
#include <string>
#include <thread>

#include <boost/asio.hpp>

int main(int argc, char *argv[]) {

  try {
    if(argc != 4) {
      std::cerr << "Invalid amount of arguments. Intended usage: ./launch_client <host_ip> <server_port> <username>" << std::endl;
      return 1;
    }
    
    std::string host_ip(argv[1]);
    std::string server_port(argv[2]);
    std::string user_name(argv[3]);
    if(user_name.size() < 10)
      while(user_name.size() < 10)
        user_name.push_back(' ');
    else if(user_name.size() > 10)
      while(user_name.size() > 10)
        user_name.pop_back();

    

    std::cout << "Will attempt to connect to server with IPv4 address: "
              << host_ip << ":" << server_port 
              << ", under the username " << user_name << "." << std::endl;

    boost::asio::io_context io_context;

    Client client(io_context, host_ip, server_port, user_name);
    
  }
  catch(std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}