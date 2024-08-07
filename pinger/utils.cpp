#include "utils.hpp"
#include <cstdint>
#include <cstring>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__unix__)
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

namespace pinger
{
std::uint16_t network_to_host_short(std::uint16_t val)
{
#if defined(_WIN32)
    return ntohs(val);
#elif defined(__unix__)
    return ::ntohs(val);
#endif
}

std::uint16_t host_to_network_short(std::uint16_t val)
{
#if defined(_WIN32)
    return htons(val);
#elif defined(__unix__)
    return ::htons(val);
#endif
}

std::uint32_t network_to_host_long(std::uint32_t val)
{
#if defined(_WIN32)
    return ntohl(val);
#elif defined(__unix__)
    return ::ntohl(val);
#endif
}

std::uint32_t host_to_network_long(std::uint32_t val)
{
#if defined(_WIN32)
    return htonl(val);
#elif defined(__unix__)
    return ::htonl(val);
#endif
}

std::uint32_t get_ip_address(const std::string &address_str)
{
#if defined(_WIN32)
    return 0;
#elif defined(__unix__)
    ::addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_ICMP;
    hints.ai_socktype = SOCK_RAW;

    ::addrinfo* result;
    auto ret = ::getaddrinfo(address_str.c_str(), "icmp", &hints, &result);
    if (ret < 0)
    {
        return 0;
    }
    std::uint32_t ret_address = 0;
    for (::addrinfo* p = result; p != nullptr; p = p->ai_next)
    {
        int sfd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sfd < 0)
        {
            continue;
        }
        if (::connect(sfd, p->ai_addr, p->ai_addrlen) == 0)
        {
            ::close(sfd);
            ::sockaddr_in* addr = reinterpret_cast<::sockaddr_in*>(p->ai_addr);
            ret_address = addr->sin_addr.s_addr;
            break;
        }
        ::close(sfd);
    }
    ::freeaddrinfo(result);
    return ret_address;
#endif
}

}   // namespace pinger
