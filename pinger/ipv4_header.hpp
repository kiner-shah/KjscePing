#pragma once

#include <cstdint>
#include <array>
#include <ostream>
#include <istream>

namespace pinger
{
constexpr std::uint8_t IPV4_HEADER_SIZE_EXCLUDING_OPTIONS_IN_BYTES = 20;
constexpr std::uint8_t IPV4_HEADER_OPTIONS_SIZE_IN_BYTES = 40;
constexpr std::uint8_t IPV4_HEADER_TOTAL_SIZE = 60;

constexpr std::uint8_t IPV4_PROTOCOL_NUMBER_ICMP = 1;
// For Network operations, administration and management (OAM), example: ping, ssh, telnet, etc.
constexpr std::uint8_t IPV4_DSCP_CS2 = 16;

struct alignas(4) ipv4_header
{
    // first 4-bits contain version, rest 4-bits contain IHL (Internet Header Length)
    std::uint8_t version_and_ihl;
    // first 6-bits contain DSCP (Differentiated Services Code Point), rest 2-bits contain ECN (Explicit Congestion Notification)
    std::uint8_t dscp_and_ecn;
    std::uint16_t total_length;
    std::uint16_t identification;
    // first 3-bits contain flags, rest 13-bits contain fragment offset
    // Flags are in order (left-to-right): More Fragments, Don't Fragment and Reserved (always 0)
    std::uint16_t flags_and_fragment_offset;
    std::uint8_t time_to_live;
    std::uint8_t protocol;
    std::uint16_t header_checksum;
    std::uint32_t source_address;
    std::uint32_t destination_address;
    std::array<std::uint8_t, IPV4_HEADER_OPTIONS_SIZE_IN_BYTES> options;

    ipv4_header(const std::uint32_t& source_addr, const std::uint32_t& destination_addr, const std::uint8_t& data_size, const std::uint8_t& ttl);
    std::uint8_t get_version() const;
    std::uint32_t get_ihl() const;

    std::array<std::uint8_t, IPV4_HEADER_TOTAL_SIZE> get_bytes() const;

    friend std::ostream& operator<<(std::ostream& os, const ipv4_header& header);
    friend std::istream& operator>>(std::istream& is, ipv4_header& header);

private:
    std::uint16_t compute_checksum();
};
}