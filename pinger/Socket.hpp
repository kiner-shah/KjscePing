#pragma once

#include <cstddef>

namespace pinger
{
enum class SocketError
{
};
struct Socket
{
    virtual ~Socket() = default;
    virtual SocketError send(const char* buffer, std::size_t buffer_length) = 0;
    virtual SocketError recv(char* buffer, std::size_t buffer_length) = 0;
};
}   // namespace pinger