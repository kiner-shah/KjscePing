#include "Socket.hpp"
#include "PosixRawSocket.hpp"

namespace pinger
{
std::unique_ptr<Socket> create_socket()
{
    return std::make_unique<PosixRawSocket>();
}
}   // namespace pinger