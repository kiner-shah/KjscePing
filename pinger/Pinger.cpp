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
    : m_callback(std::move(callback)), m_conf{conf}, m_loop_counter{0}, m_identifier{get_process_id()}
    , m_source_ip_address{get_ip_address("172.23.94.231")}
    , m_dest_ip_address{get_ip_address(conf.destination_address)}
{
    m_socket = create_socket();
    auto ret = m_socket->connect(m_dest_ip_address);
    if (ret.code().value() != 0)
    {
        std::cerr << "Connect failed [" << ret.code().value() << "] " << ret.code().message() << '\n';
        m_socket.reset();
        exit(EXIT_FAILURE);
    }

    if (m_conf.count > 0)
    {
        m_loop_condition = [this]() mutable {
            return m_loop_counter++ < m_conf.count;
        };
    }
    else
    {
        m_loop_condition = [this]() {
            return !m_is_stopped.load(std::memory_order_relaxed);
        };
    }
    m_receive_thread = std::thread([this]() {
        while (!m_is_stopped)
        {
            if (!m_is_request_sent)
            {
                continue;
            }
            std::array<char, 256> buffer;
            std::fill(buffer.begin(), buffer.end(), 0);
            int bytes_recv = 0;
            auto ret = m_socket->recv(buffer.data(), buffer.size(), bytes_recv);

            m_is_request_sent = false;
            // If ok
            if (ret.code().value() == 0)
            {
                std::stringstream s;
                s.write(buffer.data(), bytes_recv);
                icmp_header icmp_hdr;
                ipv4_header ip_hdr;
#if defined(_WIN32)
                // TODO: instead add a function in Socket for checking if it's a raw socket
                if (!(s >> ip_hdr))
                {
                    std::cerr << "Something went wrong in IP header parse\n";
                }
#endif
                if (!(s >> icmp_hdr))
                {
                    std::cerr << "Something went wrong in ICMP header parse\n";
                }
                if (icmp_hdr.type == ICMP_TYPE_ECHO_REPLY
                        && icmp_hdr.identifier == m_identifier
                        && icmp_hdr.sequence_number == m_sequence_number)
                {
                    m_cv.notify_one();
                    auto time_taken_for_response = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - m_request_start_time).count();
                    std::cout << "ECHO RESPONSE took " << time_taken_for_response
                        << " ms, got " << bytes_recv << " bytes"
                        << " sequence_no=" << icmp_hdr.sequence_number << '\n';
                }
                else
                {
                    std::cerr << "Something went wrong in ICMP response validation\n";
                }
            }
            else
            {
                if (ret.code().value() == static_cast<int>(std::errc::timed_out))
                {
                    continue;
                }
                std::cerr << "Recv failed [" << ret.code().value() << "] " << ret.code().message() << '\n';
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
    // TODO: put send operation in a separate thread
    using namespace std::chrono_literals;

    const std::string message = "Hello from \"pinger\"";
    const std::vector<std::uint8_t> message_bytes{message.begin(), message.end()};

    while (m_loop_condition())
    {
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
                                .set_protocol(IPV4_PROTOCOL_NUMBER_ICMP)
                                .finalize_build();

        std::stringstream s;
        s << icmp_hdr << message;
        const std::string buffer = s.str();

        // Send
        std::cout << "ECHO REQUEST to " << get_ip_string(m_dest_ip_address) << '\n';

        int bytes_sent = 0;
        auto ret = m_socket->send(buffer.c_str(), buffer.length(), bytes_sent);
        if (ret.code().value() != 0)
        {
            std::cerr << "Send failed [" << ret.code().value() << "] " << ret.code().message() << '\n';
            exit(EXIT_FAILURE);
        }
        else
        {
            m_is_request_sent = true;
            m_request_start_time = std::chrono::steady_clock::now();
            // Wait for 5 seconds for response
            std::unique_lock<std::mutex> lock(m_mutex);
            std::cv_status status = m_cv.wait_for(lock, 5s);
            if (status == std::cv_status::timeout)
            {
                std::cout << "TIMEOUT\n";
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
}

void Pinger::stop()
{
    if (!m_is_stopped)
    {
        m_is_stopped = true;
    }
    m_socket->disconnect();
}
} // namespace pinger