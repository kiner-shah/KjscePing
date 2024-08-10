#if defined(_WIN32)
#include "WindowsRawSocket.hpp"
#include "utils.hpp"
#include <system_error>
#include <iostream>
#include <array>

namespace pinger
{
namespace
{
std::string get_error_message_from_error_code(int error_code)
{
    std::array<char, 256> message_buffer;
    std::fill(message_buffer.begin(), message_buffer.end(), 0);

    FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,   // flags
            nullptr,                                     // lpsource
            error_code,                                  // message id
            MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),  // languageid
            message_buffer.data(),                       // output buffer
            message_buffer.size(),                       // size of msgbuf, bytes
            nullptr);                                    // va_list of arguments
    
    return std::string(message_buffer.begin(), message_buffer.end());
}
}   // namespace
WindowsRawSocket::WindowsRawSocket()
{
    WSADATA wsa_data;
    int ret = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (ret != 0)
    {
        std::cerr << "WSAStartup failed with error [" << ret << "] " << get_error_message_from_error_code(ret);
        exit(EXIT_FAILURE);
    }

    m_sock = ::socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (m_sock == INVALID_SOCKET)
    {
        int ret = WSAGetLastError();
        std::cerr << "Socket creation failed [" << ret << "] " << get_error_message_from_error_code(ret) << '\n';
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

WindowsRawSocket::~WindowsRawSocket()
{
    if (m_sock != INVALID_SOCKET)
    {
        ::closesocket(m_sock);
        WSACleanup();
    }
}

std::system_error WindowsRawSocket::connect(const std::uint32_t &destination_address)
{
    m_dest_addr.sin_family = AF_INET;
    m_dest_addr.sin_addr.S_un.S_addr = destination_address;
    m_dest_addr.sin_port = host_to_network_short(0);
    std::cerr << "Connect to " << inet_ntoa(m_dest_addr.sin_addr) << '\n';

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = host_to_network_short(get_process_id());
    addr.sin_addr.S_un.S_addr = host_to_network_long(INADDR_ANY);

    // ICMP header identifier is the port that the socket has bound to.
    // We want to use current process id for ICMP header identifier,
    // so we set the port above to current process id. And then we bind
    // to that port.
    auto ret = ::bind(m_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (ret == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        return std::system_error(std::make_error_code(static_cast<std::errc>(err)), get_error_message_from_error_code(err));
    }

    ret = ::connect(m_sock, reinterpret_cast<sockaddr*>(&m_dest_addr), sizeof(m_dest_addr));
    if (ret == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        return std::system_error(std::make_error_code(static_cast<std::errc>(err)), get_error_message_from_error_code(err));
    }
    return std::system_error(std::error_code());
}

std::system_error WindowsRawSocket::send(const char *buffer, std::size_t buffer_length, int &bytes_sent)
{
    auto ret = ::sendto(m_sock, buffer, buffer_length, 0, reinterpret_cast<sockaddr*>(&m_dest_addr), sizeof(m_dest_addr));
    if (ret == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        return std::system_error(std::make_error_code(static_cast<std::errc>(err)), get_error_message_from_error_code(err));
    }
    return std::system_error(std::error_code());
}

std::system_error WindowsRawSocket::recv(char *buffer, std::size_t buffer_length, int &bytes_recv)
{
    sockaddr recv_from_address;
    socklen_t recv_from_address_length;
    auto ret = ::recv(m_sock, buffer, buffer_length, 0);
    if (ret == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        return std::system_error(std::make_error_code(static_cast<std::errc>(err)), get_error_message_from_error_code(err));
    }
    bytes_recv = ret;
    return std::system_error(std::error_code());
}

std::system_error WindowsRawSocket::disconnect()
{
    auto ret = ::shutdown(m_sock, SD_SEND);
    if (ret == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        return std::system_error(std::make_error_code(static_cast<std::errc>(err)), get_error_message_from_error_code(err));
    }
    //::closesocket(m_sock);
    return std::system_error(std::error_code());
}
} // namespace pinger
#endif  // if defined(_WIN32)