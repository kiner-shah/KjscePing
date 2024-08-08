#include "Socket.hpp"
#include "PosixDatagramSocket.hpp"

namespace pinger
{
std::unique_ptr<Socket> create_socket()
{
    return std::make_unique<PosixDatagramSocket>();
}
}   // namespace pinger