#pragma once
#include <functional>
#include <atomic>

namespace pinger
{

struct PingerConfig
{
    std::string destination_address;
    // Number of ping requests to send, 0 means infinite
    std::uint32_t count = 0;
};

class Pinger
{
    using PingerCallbackOnNetworkChange = std::function<void(bool)>;

    PingerCallbackOnNetworkChange m_callback;
    PingerConfig m_conf;
    std::atomic_bool is_recv_thread_running {false};

public:

    Pinger(const PingerConfig& conf, PingerCallbackOnNetworkChange callback);
    void start();
    void stop();
};
}   // namespace pinger