#ifndef ASYNCHRONOUS_SERVER_CHAT_HPP
#define ASYNCHRONOUS_SERVER_CHAT_HPP

#include "threadsafe_data_structures.hpp"

#include <string>

#include <boost/asio.hpp>

class ChatMessage {

};

class ChatRoom {
public:
  

private:
  ThreadsafeSet<std::string> participants_;
};

#endif // ASYNCHRONOUS_SERVER_CHAT_HPP