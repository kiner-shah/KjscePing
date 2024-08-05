#include "utils.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

int main()
{
    pinger::icmp_header header{pinger::ICMP_TYPE_ECHO_REQUEST, 0, 0, 1234, 1};
    std::string message = "Hello from pinger";

    pinger::ipv4_header ipv4_header{0x7f000001, 0x7f000001, static_cast<std::uint8_t>(sizeof(header) + message.length()), 120};
    for (auto x : ipv4_header.get_bytes())
    {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(x) << ' ';
    }
    std::cout << '\n' << std::dec;

    header.checksum = pinger::compute_icmp_checksum(header, std::vector<char>{message.begin(), message.end()});

    std::cout << "IPv4 Header Size: " << sizeof(pinger::ipv4_header) << '\n';
    std::cout << "IPv4 Header Checksum: " << ipv4_header.header_checksum << '\n';
    std::cout << "ICMP Header Size: " << sizeof(pinger::icmp_header) << '\n';
    std::cout << "ICMP Checksum: " << header.checksum << '\n';
    std::stringstream s;
    s << header;
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
