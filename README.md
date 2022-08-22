# C++ Asynchronous Chat Server

https://user-images.githubusercontent.com/32599626/185932014-5c554b0c-90fd-4aba-87fb-b0f48ebf0d6b.mp4

Based on the examples from [boost::asio](https://www.boost.org/doc/libs/1_80_0/doc/html/boost_asio/examples/cpp11_examples.html). 

As the name indicates, this is an asynchronous chat server that was implemented in an attempt for me to learn more about Concurrency and Networking. 

The server can handle multiple clients sending messages asynchronously. Each client is identified by a username of up to 10 characters and their IP address and port. The Transmission Control Protocol (TCP) is used to provide a connection-oriented, reliable chat service. It works within the same local network, other tests were not carried out.

Each server opens a chat room, where clients can see the messages that every other client has sent. Furthermore, clients can send private messages by typing a command like:
```bash
```to <ip_address>:<port>
```
Naturally, `<ip_address>:<port>` designates the IP address and port of the receiver.

It is also possible for any client that joins a chat room to see the a number of the previously non-private messages sent in that chat room.

In order to manage the active sessions in a safe way, a threadsafe hash map structure is used, adapted from the one in Anthony Williams' "C++ Concurrency in Action".


## How to Install

Start by cloning this repository to your local machine.

Then create an [Anaconda](https://docs.anaconda.com/anaconda/install/index.html) environment with the required dependencies:

```bash
conda env create -f environment.yaml
conda activate async-server
```

Afterwards, build the project using the script that is provided here, from the `./asynchronous_server` directory:
```bash
bash build.sh
```

## How to Use

### Server
To launch a server listening on port `<port>`, run:
```bash
./build/launch_server <port>
```

The server has three commands: `active`, `exit`, and `help`. The first, prints the IP addresses and ports of the active clients. The second closes the server. The third prints out what each option does.

### Client

To launch a client that connects to a server with IP address `<ip>`, listening on port `<port>`, with username `<user_name>`, run:
```bash
./build/launch_client <ip> <port> <user_name>
```
The user name can have up to 10 characters.

Multiple clients can be connected to the same server and send public messages.
As previously mentioned, private messages can be sent using
```bash
```to <ip_address>:<port>
```

Finally, a client can exit a session by writing the following command:
```
```exit
```

