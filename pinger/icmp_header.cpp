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
}   // namespace pinger