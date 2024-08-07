#pragma once

#include <string>

namespace pinger
{
std::uint16_t network_to_host_short(std::uint16_t val);
std::uint16_t host_to_network_short(std::uint16_t val);

std::uint32_t network_to_host_long(std::uint32_t val);
std::uint32_t host_to_network_long(std::uint32_t val);

std::uint32_t get_ip_address(const std::string& address_str);
}   // namespace pinger
