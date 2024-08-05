#include "ipv4_header.hpp"
#include <algorithm>

namespace pinger
{
ipv4_header::ipv4_header(
    const std::uint32_t& source_addr,
    const std::uint32_t& destination_addr,
    const std::uint8_t& data_size,
    const std::uint8_t& ttl)
    : version_and_ihl{0x45}, dscp_and_ecn{IPV4_DSCP_CS2 << 2},
    total_length{static_cast<std::uint16_t>(IPV4_HEADER_TOTAL_SIZE + data_size)},  identification{0},
    flags_and_fragment_offset{0}, time_to_live{ttl},
    protocol{IPV4_PROTOCOL_NUMBER_ICMP}, header_checksum{},
    source_address{source_addr}, destination_address{destination_addr},
    options{0}
{
    header_checksum = compute_checksum();
}

std::uint8_t ipv4_header::get_version() const
{
    return (version_and_ihl >> 4) & 0b00001111;
}

std::uint32_t ipv4_header::get_ihl() const
{
    return (version_and_ihl & 0b00001111) * 4;
}

std::array<std::uint8_t, IPV4_HEADER_TOTAL_SIZE> ipv4_header::get_bytes() const
{
    std::array<std::uint8_t, IPV4_HEADER_TOTAL_SIZE> buffer;
    std::fill_n(buffer.begin(), buffer.size(), 0);
    std::copy_n(reinterpret_cast<char*>(const_cast<ipv4_header*>(this)), IPV4_HEADER_SIZE_EXCLUDING_OPTIONS_IN_BYTES, buffer.begin());
    std::copy_n(this->options.begin(), IPV4_HEADER_OPTIONS_SIZE_IN_BYTES, buffer.begin() + IPV4_HEADER_SIZE_EXCLUDING_OPTIONS_IN_BYTES);
    return buffer;
}

std::uint16_t ipv4_header::compute_checksum()
{
    std::uint32_t sum = 0;
    // We need to compute sum of 16-bit values, so need to pack both in one 16-bit variable
    sum += (version_and_ihl << 8) + dscp_and_ecn;
    sum += total_length + identification + flags_and_fragment_offset;
    sum += (time_to_live << 8) + protocol;
    sum += header_checksum;
    sum += source_address;
    sum += destination_address;

    for (std::size_t i = 0; i < options.size(); i += 2)
    {
        sum += (options.at(i) << 8);
        if (i + 1 < options.size())
        {
            sum += options.at(i + 1);
        }
    }
    // Now sum is 32-bit, so we need to add top 16-bits to bottom 16-bits in order
    // to have a 16-bit value;
    std::uint32_t temp_sum = (sum >> 16);
    temp_sum += static_cast<std::uint16_t>(sum);

    // Due to above addition, maybe some carry was introduced in top 16-bits, need to
    // handle add the carry to bottom 16-bits
    temp_sum += (temp_sum >> 16);

    // Now we strip off the top 16-bits, leaving behind bottom 16-bits
    // And we perform 1s complement operation on the bottom 16-bits
    std::uint16_t checksum = ~static_cast<std::uint16_t>(temp_sum);
    return checksum;
}

std::ostream& operator<<(std::ostream& os, const ipv4_header& header)
{
    os << header.version_and_ihl << header.dscp_and_ecn << header.total_length << header.identification
        << header.flags_and_fragment_offset << header.time_to_live << header.protocol << header.header_checksum
        << header.source_address << header.destination_address;
    for (const std::uint8_t& option : header.options)
    {
        os << option;
    }
    return os;
}
std::istream& operator>>(std::istream& is, ipv4_header& header)
{
    std::array<char, IPV4_HEADER_TOTAL_SIZE> buffer;
    if (!is.read(buffer.data(), IPV4_HEADER_SIZE_EXCLUDING_OPTIONS_IN_BYTES))
    {
        return is;
    }
    header.version_and_ihl = buffer[0];
    if (header.get_version() != 4)
    {
        is.setstate(std::ios::failbit);
        return is;
    }
    auto options_length = header.get_ihl() - IPV4_HEADER_SIZE_EXCLUDING_OPTIONS_IN_BYTES;
    if (options_length < 0 || options_length > IPV4_HEADER_OPTIONS_SIZE_IN_BYTES)
    {
        is.setstate(std::ios::failbit);
    }
    else
    {
        is.read(buffer.data() + IPV4_HEADER_SIZE_EXCLUDING_OPTIONS_IN_BYTES, IPV4_HEADER_OPTIONS_SIZE_IN_BYTES);
    }
    header.dscp_and_ecn = buffer[1];
    header.total_length = (buffer[2] << 8) | buffer[3];
    header.identification = (buffer[4] << 8) | buffer[5];
    header.flags_and_fragment_offset = (buffer[6] << 8) | buffer[7];
    header.time_to_live = buffer[8];
    header.protocol = buffer[9];
    header.header_checksum = (buffer[10] << 8) | buffer[11];
    std::copy_n(buffer.data() + 12, 4, &header.source_address);
    std::copy_n(buffer.data() + 16, 4, &header.destination_address);
    std::copy_n(buffer.data() + 20, IPV4_HEADER_OPTIONS_SIZE_IN_BYTES, header.options.begin());

    return is;
}
}   // namespace pinger