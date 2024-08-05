#include "utils.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

int main()
{
    std::string message = "Hello from pinger";

    pinger::icmp_header icmp_header = pinger::icmp_header_builder{}
                                        .set_type(pinger::ICMP_TYPE_ECHO_REQUEST)
                                        .set_identifier(1234)
                                        .set_sequence_number(1)
                                        .set_payload(std::vector<std::uint8_t>{message.begin(), message.end()})
                                        .finalize_build();

    pinger::ipv4_header ipv4_header = pinger::ipv4_header_builder{}
                                        .set_source_address(0x7f000001)
                                        .set_destination_address(0x7f000001)
                                        .set_payload_length(sizeof(icmp_header) + message.length())
                                        .set_ttl(120)
                                        .finalize_build();
    for (auto x : ipv4_header.get_bytes())
    {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(x) << ' ';
    }
    std::cout << '\n' << std::dec;

    std::cout << "IPv4 Header Size: " << sizeof(pinger::ipv4_header) << '\n';
    std::cout << "IPv4 Header Checksum: " << ipv4_header.header_checksum << '\n';
    std::cout << "ICMP Header Size: " << ipv4_header.get_ihl() << '\n';
    std::cout << "ICMP Checksum: " << icmp_header.checksum << '\n';
    std::stringstream s;
    s << icmp_header;
    std::string bytes = s.str();
    std::cout << "ICMP hexdump\n";
    for (char c : bytes)
    {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(c) << ' ';
    }
    std::cout << '\n';
    std::cout << "ICMP message (not all characters are printable)\n";
    for (char c : bytes)
    {
        std::cout << std::setfill(' ') << std::setw(2) << c << ' ';
    }
    std::cout << '\n';
}
