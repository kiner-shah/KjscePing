#include "icmp_header.hpp"

namespace pinger
{
std::ostream& operator<<(std::ostream& os, const icmp_header& header)
{
    os << header.type << header.code << header.checksum << header.identifier << header.sequence_number;
    return os;
}
std::istream& operator>>(std::istream& is, icmp_header& header)
{
    is >> header.type >> header.code >> header.checksum >> header.identifier >> header.sequence_number;
    return is;
}
icmp_header_builder &icmp_header_builder::set_type(const std::uint8_t &type_)
{
    m_header.type = type_;
    return *this;
}
icmp_header_builder &icmp_header_builder::set_code(const std::uint8_t &code_)
{
    m_header.code = code_;
    return *this;
}
icmp_header_builder &icmp_header_builder::set_identifier(const std::uint16_t &id)
{
    m_header.identifier = id;
    return *this;
}
icmp_header_builder &icmp_header_builder::set_sequence_number(const std::uint16_t &seq_no)
{
    m_header.sequence_number = seq_no;
    return *this;
}

icmp_header_builder &icmp_header_builder::set_payload(const std::vector<std::uint8_t> &payload)
{
    m_payload = payload;
    return *this;
}

std::uint16_t icmp_header_builder::compute_icmp_checksum()
{
    std::uint32_t sum = 0;
    // Type and Code are 8 bits, and we need to compute
    // sum of 16-bit values, so need to pack both in one 16-bit variable
    // Thus, shifting Type by 8 bits to left
    sum += (m_header.type << 8) + m_header.code;
    // Identifier and Sequence Number are both 16 bits
    sum += m_header.identifier;
    sum += m_header.sequence_number;

    for (std::size_t i = 0; i < m_payload.size(); i += 2)
    {
        sum += (m_payload.at(i) << 8);
        if (i + 1 < m_payload.size())
        {
            sum += m_payload.at(i + 1);
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

icmp_header icmp_header_builder::finalize_build()
{
    m_header.checksum = compute_icmp_checksum();
    return m_header;
}

} // namespace pinger