#include "server.hpp"
#include "constants.hpp"

#include <iostream>
#include <exception>


using std::cout;
using std::endl;

int main(int argc, char *argv[]) {

  int server_port = DEFAULT_PORT;
  try {
    // can add more error checking for whether the porpt number is valid
    if(argc == 2)
      server_port = std::stoi(argv[1]);
    else if(argc > 2) {
      std::cerr << "Invalid amount of arguments. Intended usage: ./launch_server <server_port>" << std::endl;
      return 0;
    }

    cout << server_port << endl;

    // Server server;
  }
  catch(std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}