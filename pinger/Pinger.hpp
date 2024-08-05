#pragma once
#include <functional>

namespace pinger
{

struct PingerConfig
{
    // Number of ping requests to send, -1 means infinite
    std::int32_t count = -1;
};

class Pinger
{
public:
    using PingerCallbackOnNetworkChange = std::function<void(bool)>;

    Pinger(const PingerConfig& conf, PingerCallbackOnNetworkChange callback);
    void start();
    void stop();
};
}   // namespace pinger