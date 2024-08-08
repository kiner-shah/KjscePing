#include "PosixDatagramSocket.hpp"
#include "utils.hpp"
#include <cstring>
#include <system_error>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

namespace pinger
{
PosixDatagramSocket::PosixDatagramSocket()
{
    m_sock_fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if (m_sock_fd < 0)
    {
        std::cerr << "Socket creation failed [" << errno << "] " << strerror(errno) << '\n';
        exit(EXIT_FAILURE);
    }
}

PosixDatagramSocket::~PosixDatagramSocket()
{
    if (m_sock_fd > 0)
    {
        ::close(m_sock_fd);
    }
}

std::system_error PosixDatagramSocket::connect(const std::uint32_t& destination_address)
{
    m_dest_addr.sin_family = AF_INET;
    m_dest_addr.sin_addr.s_addr = destination_address;
    m_dest_addr.sin_port = host_to_network_short(0);
    std::cerr << "Connect to " << inet_ntoa(m_dest_addr.sin_addr) << '\n';

    ::sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = get_process_id();
    addr.sin_addr.s_addr = INADDR_ANY;

    // ICMP header identifier is the port that the socket has bound to.
    // We want to use current process id for ICMP header identifier,
    // so we set the port above to current process id. And then we bind
    // to that port.
    auto ret = ::bind(m_sock_fd, reinterpret_cast<::sockaddr*>(&addr), sizeof(addr));
    if (ret < 0)
    {
        return std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
    }

    ret = ::connect(m_sock_fd, reinterpret_cast<::sockaddr*>(&m_dest_addr), sizeof(m_dest_addr));
    if (ret < 0)
    {
        return std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
    }
    return std::system_error{};
}

std::system_error PosixDatagramSocket::send(const char* buffer, std::size_t buffer_length, int& bytes_sent)
{
    bytes_sent = ::sendto(m_sock_fd, buffer, buffer_length, 0, reinterpret_cast<::sockaddr*>(&m_dest_addr), sizeof(m_dest_addr));
    if (bytes_sent < 0)
    {
        return std::system_error(std::make_error_code(static_cast<std::errc>(errno)), ::strerror(errno));
    }
    return std::system_error{};
}

std::system_error PosixDatagramSocket::recv(char *buffer, std::size_t buffer_length, int& bytes_recv)
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