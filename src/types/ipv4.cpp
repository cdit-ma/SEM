//
// Created by mitchell on 7/9/20.
//

#include "ipv4.hpp"
#include <regex>
#include <stdexcept>

namespace sem::types {
namespace detail {
auto parse_octet_from_string(const std::string& in) -> uint8_t
{
    auto number = std::stoi(in);
    if(number < 0 || number > 255) {
        throw std::invalid_argument("Ip address octet is out of range ([0,255]).");
    }
    return static_cast<uint8_t>(number);
}
} // namespace detail

auto Ipv4::parse_string_to_octets(const std::string& addr_string) -> std::array<uint8_t, 4>
{
    // Break out early if we're too long to avoid pathological regex matching.
    if(addr_string.length() > 15) {
        throw std::invalid_argument("Invalid IP address string format; too long.");
    }
    // Validate ipv4 address format
    // Regex matches nnn.nnn.nnn.nnn, where n is a numeral and the number of numerals per octet
    // is 1-3. Places matches in capture groups 1-4. Note indexing from 1, full match is placed
    // in index 0.
    const std::regex addr_check{R"--(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)--"};
    std::smatch matches;
    if(!std::regex_match(addr_string.begin(), addr_string.end(), matches, addr_check)) {
        throw std::invalid_argument("Invalid IP address string format; cannot parse.");
    }

    std::array<uint8_t, 4> octets{};
    // Magic "+ 1" to skip the first match which is always the full match (i.e. the full ip
    //  addr)
    std::transform(matches.begin() + 1, matches.end(), octets.begin(),
                   detail::parse_octet_from_string);
    return octets;
}

} // namespace sem::types