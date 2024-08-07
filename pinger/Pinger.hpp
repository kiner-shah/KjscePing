#pragma once
#include <functional>
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Socket.hpp"

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
    bool m_is_network_available {false};
    PingerConfig m_conf;
    std::atomic_bool m_is_recv_thread_running {false};
    std::unique_ptr<Socket> m_socket;
    std::thread m_receive_thread;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic_int16_t m_sequence_number {0};

    const std::uint16_t m_identifier;
    const std::uint32_t m_source_ip_address;
    const std::uint32_t m_dest_ip_address;

public:
    Pinger(const PingerConfig& conf, PingerCallbackOnNetworkChange callback);
    ~Pinger();
    void start();
    void stop();
};
}   // namespace pinger