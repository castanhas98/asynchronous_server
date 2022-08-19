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
  // user_name + ip + ':' + port
  static constexpr std::size_t user_name_length_ =  10 + 15 + 1 + 5;


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

  void encode_sender(char* user_name, char* ip_and_port);

  // void fill(const char c);

private:
  char data_[header_length_ + user_name_length_ + max_body_length_];
  // std::pair<std::string, std::string> nickname_ip_;
  std::size_t body_length_;
};

class ChatParticipant {
public:
  virtual ~ChatParticipant() {}
  virtual void deliver(const ChatMessage& msg) = 0;
  // DELETE
  // virtual boost::asio::ip::address get_endpoint_ip_address() = 0;
  // virtual boost::asio::ip::port_type get_endpoint_port() = 0;
};

class ChatRoom {
public:
  void join(std::shared_ptr<ChatParticipant> participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_messages_)
      participant->deliver(msg.first); // msg.first is the message, msg.second is the sender
  }

  void leave(std::shared_ptr<ChatParticipant> participant)
  {
    participants_.erase(participant);
  }

  void deliver(const ChatMessage& msg, const std::shared_ptr<ChatParticipant>& sender)
  {
    recent_messages_.push_back({msg, sender});
    while (recent_messages_.size() > max_recent_msgs_)
      recent_messages_.pop_front();

    for (auto participant: participants_) {
      if(participant != sender)
        participant->deliver(msg);
    }

    // std::cout << "PRINTING PARTICIPANTS:" << std::endl;
    // for (auto participant: participants_) 
    //   std::cout << participant->get_endpoint_ip_address()
    //             << ":" << participant->get_endpoint_port()
    //             << std::endl;
  }

private:
  std::set<std::shared_ptr<ChatParticipant>> participants_; // Change to threadsafe
  static constexpr unsigned int max_recent_msgs_ = 100 ;
  // std::deque<ChatMessage> recent_messages_;
  std::deque<std::pair<ChatMessage, std::shared_ptr<ChatParticipant>>> recent_messages_;
};



#endif // ASYNCHRONOUS_SERVER_CHAT_HPP