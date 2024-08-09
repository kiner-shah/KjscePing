#pragma once
#if defined(_WIN32)
#include "Socket.hpp"
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>

namespace pinger
{
class WindowsDatagramSocket : public Socket
{
    SOCKET m_sock;
    sockaddr_in m_dest_addr;
public:
    WindowsDatagramSocket();
    ~WindowsDatagramSocket();
    std::system_error connect(const std::uint32_t& destination_address) override;
    std::system_error send(const char* buffer, std::size_t buffer_length, int& bytes_sent) override;
    std::system_error recv(char* buffer, std::size_t buffer_length, int& bytes_recv) override;
    std::system_error disconnect() override;
};
}   // namespace pinger
#endif  // if defined(_WIN32) 