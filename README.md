# Asynchronous Server

Ideas:
- `top`-like command, that says how many clients are using the server.
- Supporting tcp and udp clients.
- Voice udp broadcast or multicast.

To do (High to Low priority timeline-wise):
  - Chat between clients:
    - Chat between two clients
      - Sessions are managed by the server?
      - Client message needs to have a header and a body:
          - header can have type of message (exit, send) and destination (ip:port), in case of a send message. 
          - Body can have whatever.
          - Exit message deletes the session from the client's set and closes it.
      - Creating a chat session that many clients can join
    - Client UI
  - Thread-safe session bookkeeping data structure
  