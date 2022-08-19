#include "chat.hpp"

ChatMessage::ChatMessage()
: body_length_(0) {}

std::size_t ChatMessage::length() const {
  return header_length_ + body_length_;
}

const char* ChatMessage::body() const {
  return data_ + header_length_; // change these when there are headers
}

char* ChatMessage::body() {
  return data_ + header_length_; // change these when there are headers
}

const char* ChatMessage::data() const {
  return data_;
}

char* ChatMessage::data() {
  return data_;
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
  std::memcpy(data_, user_name, 10);
  std::memcpy(data_ + 10, ip_and_port, 21);
}

// void ChatMessage::fill(const char c) {
//   data_.fill(c);
// }