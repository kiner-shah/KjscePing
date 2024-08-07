#include "PosixRawSocket.hpp"
#include <cstring>
#include <system_error>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

namespace pinger
{
PosixRawSocket::PosixRawSocket()
{
    m_sock_fd = ::socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (m_sock_fd < 0)
    {
        std::cerr << "Socket creation failed [" << errno << ']' << strerror(errno) << '\n';
        exit(EXIT_FAILURE);
    }
}

PosixRawSocket::~PosixRawSocket()
{
    if (m_sock_fd > 0)
    {
        ::close(m_sock_fd);
    }
}

std::system_error PosixRawSocket::connect(const std::string &destination_address)
{
    m_dest_addr.sin_family = AF_INET;
    m_dest_addr.sin_addr.s_addr = 0x7f000001;   // TODO: convert destination_address to int
    auto ret = ::connect(m_sock_fd, reinterpret_cast<::sockaddr*>(&m_dest_addr), sizeof(m_dest_addr));
    if (ret < 0)
    {
        return std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
    }
    return std::system_error{};
}

std::system_error PosixRawSocket::send(const char* buffer, std::size_t buffer_length, int& bytes_sent)
{
    bytes_sent = ::sendto(m_sock_fd, buffer, buffer_length, 0, reinterpret_cast<::sockaddr*>(&m_dest_addr), sizeof(m_dest_addr));
    if (bytes_sent < 0)
    {
        return std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
    }
    return std::system_error{};
}

std::system_error PosixRawSocket::recv(char *buffer, std::size_t buffer_length, int& bytes_recv)
{
    ::sockaddr recv_from_address;
    ::socklen_t recv_from_address_length;
    bytes_recv = ::recvfrom(m_sock_fd, buffer, buffer_length, MSG_WAITALL, &recv_from_address, &recv_from_address_length);
    if (bytes_recv < 0)
    {
        return std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
    }
    return std::system_error{};
}

}   // namespace pinger