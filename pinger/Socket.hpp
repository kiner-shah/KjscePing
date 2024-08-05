#pragma once

#include <cstddef>

namespace pinger
{
struct Socket
{
    virtual ~Socket() = default;
    virtual bool send(const char* buffer, std::size_t buffer_length) = 0;
    virtual bool recv(char* buffer, std::size_t buffer_length) = 0;
};
}   // namespace pinger