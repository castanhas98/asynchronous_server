#ifndef ASYNCHRONOUS_SERVER_CLIENT_HPP
#define ASYNCHRONOUS_SERVER_CLIENT_HPP

#include "chat.hpp"

#include <array>
#include <deque>
#include <string>

#include <boost/asio.hpp>

class Client {
  public:
    Client(
      boost::asio::io_context& io_context, 
      std::string& server_ip,
      std::string& server_port
    );

    ~Client();

  private:
    void do_echo();

    void do_connect(std::string& server_ip, std::string& server_port);

    void do_read_header();

    void do_read_body();

    void do_get_messages();

    void write(const ChatMessage& msg);

    void do_write();

  private:
    boost::asio::io_context& io_context_;

    boost::asio::ip::tcp::socket tcp_socket_;

    boost::asio::ip::tcp::resolver tcp_resolver_;

    std::array<char, 1024> message_;

    std::array<char, 1024> reply_;

    ChatMessage read_msg_;

    std::deque<ChatMessage> write_msgs_;

    std::thread io_context_thread_;


};

#endif // ASYNCHRONOUS_SERVER_CLIENT_HPP