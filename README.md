# Asynchronous Server

Ideas:
- `top`-like command, that says how many clients are using the server.
- Supporting tcp and udp clients.
- Voice udp broadcast or multicast.

To do (High to Low priority timeline-wise):
  - Chat between clients:
    - Clients can have username!
    - Only print message for the other clients, don't do echo.
    - Chat between two clients
      - Sessions are managed by the server?
      - Client message needs to have a header and a body:
          - header can have type of message (exit, send) and destination (ip:port), in case of a send message. 
          - Body can have whatever.
          - Exit message deletes the session from the client's set and closes it.
      - Creating a chat session that many clients can join
    - Client UI
  - Understand why we need a default value in the thread-safe map.
  - Understand why we need to delete copy constructor and copy assignment operator. Appendix A.2.