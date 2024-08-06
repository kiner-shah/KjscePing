#include "Pinger.hpp"
#include "icmp_header.hpp"
#include "ipv4_header.hpp"
#include <thread>
#include <sstream>

namespace pinger
{
Pinger::Pinger(const PingerConfig &conf, PingerCallbackOnNetworkChange callback)
    : m_callback(std::move(callback)), m_conf{conf}
{
    // TODO: create a socket (use a factory?), call connect

    m_receive_thread = std::thread([this]() {
        m_is_recv_thread_running = true;
        while (m_is_recv_thread_running)
        {
            std::array<char, 1024> buffer;
            auto ret = m_socket->recv(buffer.data(), buffer.size());
            // TODO: if ok

            m_cv.notify_one();
        }
    });
}

Pinger::~Pinger()
{
    if (m_is_recv_thread_running)
    {
        m_is_recv_thread_running = false;
    }
    m_receive_thread.join();
}

void Pinger::start()
{
    const std::string message = "Hello from \"pinger\"";
    const std::vector<std::uint8_t> message_bytes{message.begin(), message.end()};
    std::uint16_t seq_no = 1;

    // TODO: add the below in a while loop
    const icmp_header icmp_hdr = icmp_header_builder{}
                            .set_type(ICMP_TYPE_ECHO_REQUEST)
                            .set_identifier(1234)
                            .set_sequence_number(seq_no++)
                            .set_payload(message_bytes)
                            .finalize_build();
    // TODO: fill other fields
    const ipv4_header ip_hdr = ipv4_header_builder{}
                            .set_source_address(0x7f000001)
                            .set_destination_address(0x7f000001)
                            .set_ttl(120)
                            .set_payload_length(sizeof(icmp_header) + message.length())
                            .finalize_build();
    std::stringstream s;
    s << ip_hdr << icmp_hdr;
    const std::string buffer = s.str();
    // 2. Send
    // TODO: log echo request
    auto ret = m_socket->send(buffer.c_str(), buffer.length());
    // TODO: if not ok
    //    take appropriate action
    // else
    // 3. Wait for 5 seconds for response
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::cv_status status = m_cv.wait_for(lock, std::chrono::seconds(5));
        if (status == std::cv_status::timeout)
        {
            // TODO: log timeout
            if (m_is_network_available)
            {
                m_is_network_available = false;
                m_callback(m_is_network_available);
            }
        }
        else
        {
            // TODO: log echo response
            if (!m_is_network_available)
            {
                m_is_network_available = true;
                m_callback(m_is_network_available);
            }
        }
    }
    // 4. Repeat 2 & 3, until thread is stopped using a boolean flag
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Pinger::stop()
{
    m_is_recv_thread_running = false;
}
} // namespace pinger