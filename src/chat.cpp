#include "chat.hpp"

// ChatMessage

ChatMessage::ChatMessage()
: body_length_(0) {}

std::size_t ChatMessage::length() const {
  return header_length_ + user_name_length_ + body_length_;
}

const char* ChatMessage::body() const {
  return data_ + header_length_ + user_name_length_; // change these when there are headers
}

char* ChatMessage::body() {
  return data_ + header_length_ + user_name_length_; // change these when there are headers
}

const char* ChatMessage::body_with_sender() const {
  return data_ + header_length_; // change these when there are headers
}

char* ChatMessage::body_with_sender() {
  return data_ + header_length_; // change these when there are headers
}

const char* ChatMessage::data() const {
  return data_;
}

char* ChatMessage::data() {
  return data_;
}

std::size_t ChatMessage::body_with_sender_length() const {
  return user_name_length_ + body_length_;
}

std::size_t ChatMessage::body_length() const {
  return body_length_;
}

void ChatMessage::body_length(std::size_t new_length) {
  body_length_ = (new_length >= max_body_length_) ? max_body_length_ : new_length;
}

bool ChatMessage::decode_header() {
  char header[header_length_ + 1] = "";
  std::strncat(header, data_, header_length_);
  body_length_ = std::atoi(header);
  if (body_length_ > max_body_length_)
  {
    body_length_ = 0;
    return false;
  }
  return true;
}

void ChatMessage::encode_header() {
  char header[header_length_ + 1] = "";
  std::sprintf(header, "%4d", static_cast<int>(body_length_));
  std::memcpy(data_, header, header_length_);
}

void ChatMessage::encode_sender(char* user_name, char* ip_and_port) {
  std::memcpy(data_ + header_length_, user_name, 10);
  std::memcpy(data_ + header_length_ + 10, ip_and_port, user_name_length_ - 10);
}


// ChatRoom

ChatRoom::ChatRoom() {

}

void ChatRoom::join(std::shared_ptr<ChatParticipant> participant)
{
  participants_.insert(participant);
  for (auto msg: recent_messages_)
    participant->deliver(msg.first); // msg.first is the message, msg.second is the sender
}

void ChatRoom::leave(std::shared_ptr<ChatParticipant> participant)
{
  participants_.erase(participant);
}

void ChatRoom::deliver(const ChatMessage& msg, const std::shared_ptr<ChatParticipant>& sender)
{
  if(participants_.find(sender) != participants_.end()) {
    recent_messages_.push_back({msg, sender});
    while (recent_messages_.size() > max_recent_msgs_)
      recent_messages_.pop_front();

    for (auto participant: participants_) {
      if(participant != sender)
        participant->deliver(msg);
    }
  }
}

void ChatRoom::deliver_to(
  const ChatMessage& msg, 
  const std::shared_ptr<ChatParticipant>& sender, 
  std::string& target_ip, 
  std::string& target_port
) {
  for(auto& participant : participants_) {
    auto participant_port = std::to_string(participant->get_tcp_socket().remote_endpoint().port());
    auto participant_ip = participant->get_tcp_socket().remote_endpoint().address().to_string();

    if(participant_ip == target_ip && participant_port == target_port) {
      std::cout << "got to the comparison" << std::endl;
      participant->deliver(msg);
      return;
    }
  }
}
