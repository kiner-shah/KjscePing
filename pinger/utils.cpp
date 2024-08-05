#include "utils.hpp"

namespace pinger
{
std::uint16_t compute_icmp_checksum(const icmp_header& header, const std::vector<char>& body)
{
    std::uint32_t sum = 0;
    // Type and Code are 8 bits, and we need to compute
    // sum of 16-bit values, so need to pack both in one 16-bit variable
    // Thus, shifting Type by 8 bits to left
    sum += (header.type << 8) + header.code;
    // Identifier and Sequence Number are both 16 bits
    sum += header.identifier;
    sum += header.sequence_number;

    for (std::size_t i = 0; i < body.size(); i += 2)
    {
        sum += (body.at(i) << 8);
        if (i + 1 < body.size())
        {
            sum += body.at(i + 1);
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
}   // namespace pinger
