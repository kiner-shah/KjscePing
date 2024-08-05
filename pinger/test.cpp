#include "utils.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

int main()
{
    pinger::icmp_header header{pinger::ICMP_TYPE_ECHO_REQUEST, 0, 0, 1234, 1};
    std::string message = "Hello from pinger";
    std::cout << sizeof(pinger::icmp_header) << '\n';
    std::cout << pinger::compute_checksum(header, std::vector<char>{message.begin(), message.end()}) << '\n';
    std::stringstream s;
    s << header;
    std::string bytes = s.str();
    for (char c : bytes)
    {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(c) << ' ';
    }
    std::cout << '\n';
    for (char c : bytes)
    {
        std::cout << std::setfill(' ') << std::setw(2) << c << ' ';
    }
    std::cout << '\n';
}
