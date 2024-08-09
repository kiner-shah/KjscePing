#include "Socket.hpp"
#if defined(_WIN32)
#include "WindowsRawSocket.hpp"
#elif defined(__unix__)
#include "PosixDatagramSocket.hpp"
#endif

namespace pinger
{
std::unique_ptr<Socket> create_socket()
{
#if defined(_WIN32)
    return std::make_unique<WindowsRawSocket>();
#elif defined(__unix__)
    return std::make_unique<PosixDatagramSocket>();
#endif
}
}   // namespace pinger