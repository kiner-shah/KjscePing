#pragma once

#include <cstddef>
#include <system_error>
#include <memory>

namespace pinger
{
struct Socket
{
    virtual ~Socket() = default;
    virtual std::system_error connect(const std::string &destination_address) = 0;
    virtual std::system_error send(const char* buffer, std::size_t buffer_length, int& bytes_sent) = 0;
    virtual std::system_error recv(char* buffer, std::size_t buffer_length, int& bytes_recv) = 0;
};

std::unique_ptr<Socket> create_socket();
}   // namespace pinger