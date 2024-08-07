#pragma once

#include "Socket.hpp"
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>

namespace pinger
{
class PosixRawSocket : public Socket
{
    std::int32_t m_sock_fd;
    ::sockaddr_in m_dest_addr;
public:
    PosixRawSocket();
    ~PosixRawSocket();
    std::system_error connect(const std::string &destination_address) override;
    std::system_error send(const char* buffer, std::size_t buffer_length, int& bytes_sent) override;
    std::system_error recv(char* buffer, std::size_t buffer_length, int& bytes_recv) override;
};
}   // namespace pinger