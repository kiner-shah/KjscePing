# Pinger

Ping library made using C++ 14 which would send ICMP ECHO packets and receive ECHO responses.

- [pinger](./pinger) - C++ 14 library and test file
- [old](./old) - old code written in C during college days

GCC compilation:
```
g++ -g utils.cpp PosixDatagramSocket.cpp Socket.cpp ipv4_header.cpp icmp_header.cpp Pinger.cpp test.cpp -o pinger
```
