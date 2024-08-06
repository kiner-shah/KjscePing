#include "PosixRawSocket.hpp"
#include <cstring>
#include <system_error>

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

namespace pinger
{
PosixRawSocket::PosixRawSocket()
{
    m_sock_fd = ::socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
    if (m_sock_fd < 0)
    {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
    }
}

PosixRawSocket::~PosixRawSocket()
{
    if (m_sock_fd > 0)
    {
        ::close(m_sock_fd);
    }
}

bool PosixRawSocket::connect(const std::string &destination_address)
{
    m_dest_addr.sin_family = AF_INET;
    m_dest_addr.sin_addr.s_addr = 0x7f000001;   // TODO: convert destination_address to int
    auto ret = ::connect(m_sock_fd, reinterpret_cast<::sockaddr*>(&m_dest_addr), sizeof(m_dest_addr));
    if (ret < 0)
    {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
        return false;
    }
    return true;
}

bool PosixRawSocket::send(const char *buffer, std::size_t buffer_length)
{
    auto bytes_sent = ::sendto(m_sock_fd, buffer, buffer_length, 0, reinterpret_cast<::sockaddr*>(&m_dest_addr), sizeof(m_dest_addr));
    if (bytes_sent < 0)
    {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
    }
    return true;
}

bool PosixRawSocket::recv(char *buffer, std::size_t buffer_length)
{
    ::sockaddr recv_from_address;
    ::socklen_t recv_from_address_length;
    auto bytes_read = ::recvfrom(m_sock_fd, buffer, buffer_length, MSG_WAITALL, &recv_from_address, &recv_from_address_length);
    if (bytes_read < 0)
    {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
    }
    else
    {
        // Check if recv_from_address is same as m_dest_addr
        ::sockaddr_in* recv_from_address_in = reinterpret_cast<::sockaddr_in*>(&recv_from_address);
        if (recv_from_address_in->sin_family != m_dest_addr.sin_family
            || recv_from_address_in->sin_addr.s_addr != m_dest_addr.sin_addr.s_addr)
        {
            return false;
        }
        // TODO: do something with bytes_read and buffer
    }
    return true;
}

}   // namespace pinger