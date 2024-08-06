#pragma once

#include "Socket.hpp"
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>

namespace pinger
{

// Based on list in /etc/protocols
constexpr std::int32_t IP_PROTO_ICMP = 1;

class PosixRawSocket : public Socket
{
    std::int32_t m_sock_fd;
    ::sockaddr_in m_dest_addr;
public:
    PosixRawSocket();
    ~PosixRawSocket();
    bool connect(const std::string& destination_address) override;
    bool send(const char* buffer, std::size_t buffer_length) override;
    bool recv(char* buffer, std::size_t buffer_length) override;
};
}   // namespace pinger