#pragma once

#include <cstdint>
#include <ostream>
#include <istream>

namespace pinger
{

constexpr std::uint8_t ICMP_TYPE_ECHO_REPLY = 0;
constexpr std::uint8_t ICMP_TYPE_ECHO_REQUEST = 8;
constexpr std::uint8_t ICMP_TYPE_TIME_EXCEEDED = 11;

constexpr std::uint8_t ICMP_CODE_TIME_EXCEEDED_TTL_EXCEEDED = 0;
constexpr std::uint8_t ICMP_CODE_TIME_EXCEEDED_FRAG_REASSEM_TIME_EXCEEDED = 1;

struct alignas(4) icmp_header
{
    std::uint8_t type = 0;
    std::uint8_t code = 0;
    std::uint16_t checksum = 0;
    std::uint16_t identifier = 0;
    std::uint16_t sequence_number = 0;

    friend std::ostream& operator<<(std::ostream& os, const icmp_header& header)
    {
        os << header.type << header.code << header.checksum << header.identifier << header.sequence_number;
        return os;
    }
    friend std::istream& operator>>(std::istream& is, icmp_header& header)
    {
        is >> header.type >> header.code >> header.checksum >> header.identifier >> header.sequence_number;
        return is;
    }
};

}   // namespace pinger
