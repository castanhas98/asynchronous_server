# C++ Asynchronous Chat Server

https://user-images.githubusercontent.com/32599626/185932014-5c554b0c-90fd-4aba-87fb-b0f48ebf0d6b.mp4

Based on the examples from [boost::asio](https://www.boost.org/doc/libs/1_80_0/doc/html/boost_asio/examples/cpp11_examples.html). 

As the name indicates, this is an asynchronous chat server that was implemented in an attempt for me to learn more about Concurrency and Networking. 

The server can handle multiple clients sending messages asynchronously. Each client is identified by a username of up to 10 characters and their IP address and port. The Transmission Control Protocol (TCP) is used to provide a connection-oriented, reliable chat service.

Each server opens a chat room, where clients can see the messages that every other client has sent. Furthermore, clients can send private messages by typing a command like:
```bash
```to <ip_address>:<port>
```
Naturally, `<ip_address>:<port>` designates the IP address and port of the receiver.

It is also possible for any client that joins a chat room to see the a number of the previously non-private messages sent in that chat room.

In order to manage the active sessions in a safe way, a threadsafe hash map structure is used, adapted from the one in Anthony Williams' "C++ Concurrency in Action".


## Example


## How to Install


## How to Use
