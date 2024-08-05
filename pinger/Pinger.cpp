#include "Pinger.hpp"
#include "icmp_header.hpp"
#include <thread>

namespace pinger
{
Pinger::Pinger(const PingerConfig &conf, PingerCallbackOnNetworkChange callback)
    : m_callback(std::move(callback)), m_conf{conf}
{
}

void Pinger::start()
{
    // 1. Start receive thread
    // 2. Start sending ICMP messages
    // 3. Wait for 5 seconds for response
    // 4. Repeat 2 & 3, until thread is stopped using a boolean flag
}

void Pinger::stop()
{

}
} // namespace pinger