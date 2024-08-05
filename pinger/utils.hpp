#pragma once

#include "icmp_header.hpp"
#include "ipv4_header.hpp"
#include <vector>

namespace pinger
{
std::uint16_t compute_icmp_checksum(const icmp_header& header, const std::vector<char>& data);
}   // namespace pinger
