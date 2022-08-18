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


  ChatMessage();

  std::size_t length() const;

  const char* body() const;

  char* body();

  const char* data() const;

  char* data();

  std::size_t body_length() const;

  void body_length(std::size_t new_length);

  bool decode_header();

  void encode_header();

  // void fill(const char c);

private:
  char data_[max_body_length_];
  // std::pair<std::string, std::string> nickname_ip_;
  std::size_t body_length_;
};

class ChatParticipant
{
public:
  virtual ~ChatParticipant() {}
  virtual void deliver(const ChatMessage& msg) = 0;
};

class ChatRoom {
public:
  void join(std::shared_ptr<ChatParticipant> participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_messages_)
      participant->deliver(msg);
  }

  void leave(std::shared_ptr<ChatParticipant> participant)
  {
    participants_.erase(participant);
  }

  void deliver(const ChatMessage& msg)
  {
    recent_messages_.push_back(msg);
    while (recent_messages_.size() > max_recent_msgs)
      recent_messages_.pop_front();

    for (auto participant: participants_)
      participant->deliver(msg);
  }

private:
  std::set<std::shared_ptr<ChatParticipant>> participants_;
  enum { max_recent_msgs = 100 };
  std::deque<ChatMessage> recent_messages_;
};



#endif // ASYNCHRONOUS_SERVER_CHAT_HPP