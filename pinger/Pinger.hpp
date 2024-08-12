#pragma once
#include <functional>
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
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
    using LoopCondition = std::function<bool(void)>;

    PingerCallbackOnNetworkChange m_callback;
    bool m_is_network_available {false};
    PingerConfig m_conf;
    std::atomic_bool m_is_stopped {false};
    std::atomic_bool m_is_request_sent{ false };
    std::unique_ptr<Socket> m_socket;
    std::thread m_receive_thread;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic_int16_t m_sequence_number {0};
    LoopCondition m_loop_condition;
    std::uint32_t m_loop_counter;

    std::chrono::time_point<std::chrono::steady_clock> m_request_start_time;

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