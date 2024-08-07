#include "Pinger.hpp"
#include "icmp_header.hpp"
#include "ipv4_header.hpp"
#include "utils.hpp"
#include <thread>
#include <sstream>
#include <iostream>
#include <cstring>

namespace pinger
{
Pinger::Pinger(const PingerConfig &conf, PingerCallbackOnNetworkChange callback)
    : m_callback(std::move(callback)), m_conf{conf}, m_identifier{1234}
    , m_source_ip_address{get_ip_address("172.23.94.231")}
    , m_dest_ip_address{get_ip_address(conf.destination_address)}
{
    // TODO: create a socket (use a factory?), call connect
    m_socket = create_socket();

    m_receive_thread = std::thread([this]() {
        m_is_recv_thread_running = true;
        while (m_is_recv_thread_running)
        {
            std::array<char, 256> buffer;
            std::fill(buffer.begin(), buffer.end(), 0);
            int bytes_recv = 0;
            auto ret = m_socket->recv(buffer.data(), buffer.size(), bytes_recv);

            // If ok
            if (ret.code().value() == 0)
            {
                std::stringstream s;
                s.write(buffer.data(), bytes_recv);
                icmp_header icmp_hdr;
                ipv4_header ip_hdr;
                s >> ip_hdr >> icmp_hdr;
                if (s && icmp_hdr.type == ICMP_TYPE_ECHO_REPLY
                        && icmp_hdr.identifier == m_identifier
                        && icmp_hdr.sequence_number == m_sequence_number)
                {
                    m_cv.notify_one();

                    // TODO: log echo response
                }
            }
            else
            {
                std::cerr << "Recv failed [" << errno << ']' << strerror(errno) << '\n';
                // TODO: maybe add a log message?
                exit(EXIT_FAILURE);
            }
        }
    });
}

Pinger::~Pinger()
{
    stop();
    m_receive_thread.join();
}

void Pinger::start()
{
    using namespace std::chrono_literals;

    const std::string message = "Hello from \"pinger\"";
    const std::vector<std::uint8_t> message_bytes{message.begin(), message.end()};

    // TODO: add the below code in a while loop
    const icmp_header icmp_hdr = icmp_header_builder{}
                            .set_type(ICMP_TYPE_ECHO_REQUEST)
                            .set_identifier(m_identifier)
                            .set_sequence_number(++m_sequence_number)
                            .set_payload(message_bytes)
                            .finalize_build();

    const ipv4_header ip_hdr = ipv4_header_builder{}
                            .set_source_address(m_source_ip_address)
                            .set_destination_address(m_dest_ip_address)
                            .set_ttl(120)
                            .set_payload_length(sizeof(icmp_header) + message.length())
                            .finalize_build();

    std::stringstream s;
    s << ip_hdr << icmp_hdr;
    const std::string buffer = s.str();

    // Send
    // TODO: log echo request
    int bytes_sent = 0;
    auto ret = m_socket->send(buffer.c_str(), buffer.length(), bytes_sent);
    if (ret.code().value() != 0)
    {
        // TODO: failed - what to do?
        exit(EXIT_FAILURE);
    }
    else
    {
        // Wait for 5 seconds for response
        std::unique_lock<std::mutex> lock(m_mutex);
        std::cv_status status = m_cv.wait_for(lock, 5s);
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
            if (!m_is_network_available)
            {
                m_is_network_available = true;
                m_callback(m_is_network_available);
            }
        }
    }
    // Repeat until thread is stopped using a boolean flag
    std::this_thread::sleep_for(1s);
}

void Pinger::stop()
{
    if (m_is_recv_thread_running)
    {
        m_is_recv_thread_running = false;
    }
}
} // namespace pinger