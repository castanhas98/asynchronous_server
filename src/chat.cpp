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

typename std::array<char, 512>::size_type ChatMessage::body_length() const {
  return body_length_;
}

void ChatMessage::body_length(typename std::array<char, 512>::size_type new_length) {
  body_length_ = (new_length >= max_body_length_) ? max_body_length_ : new_length;
}

bool ChatMessage::decode_header()
{
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

void ChatMessage::encode_header()
{
  char header[header_length_ + 1] = "";
  std::sprintf(header, "%4d", static_cast<int>(body_length_));
  std::memcpy(data_, header, header_length_);
}

// void ChatMessage::fill(const char c) {
//   data_.fill(c);
// }