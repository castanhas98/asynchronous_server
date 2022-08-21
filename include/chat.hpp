#ifndef ASYNCHRONOUS_SERVER_CHAT_HPP
#define ASYNCHRONOUS_SERVER_CHAT_HPP

#include "threadsafe_data_structures.hpp"

#include <array>
#include <deque>
#include <string>
#include <utility>

#include <boost/asio.hpp>


class ChatMessage {
public:
  static constexpr std::size_t max_body_length_ = 512;
  static constexpr std::size_t header_length_ = 4;
  // user_name + '@' + ip + ':' + port + ": "
  static constexpr std::size_t user_name_length_ =  10 + 1 + 15 + 1 + 5 + 2;


  ChatMessage();

  std::size_t length() const;

  const char* body() const;

  char* body();

  const char* body_with_sender() const;

  char* body_with_sender();

  const char* data() const;

  char* data();

  std::size_t body_with_sender_length() const;

  std::size_t body_length() const;

  void body_length(std::size_t new_length);

  bool decode_header();

  void encode_header();

  void encode_sender(char* user_name, char* ip_and_port);

private:
  char data_[header_length_ + user_name_length_ + max_body_length_];

  std::size_t body_length_;
};

class ChatParticipant {
public:
  virtual ~ChatParticipant() {}
  virtual void deliver(const ChatMessage& msg) = 0;
};

class ChatRoom {
public:
  ChatRoom();

  void join(std::shared_ptr<ChatParticipant> participant);

  void leave(std::shared_ptr<ChatParticipant> participant);

  void deliver(const ChatMessage& msg, const std::shared_ptr<ChatParticipant>& sender);

private:
  std::set<std::shared_ptr<ChatParticipant>> participants_;

  static constexpr unsigned int max_recent_msgs_ = 100 ;

  std::deque<std::pair<ChatMessage, std::shared_ptr<ChatParticipant>>> recent_messages_;
};



#endif // ASYNCHRONOUS_SERVER_CHAT_HPP