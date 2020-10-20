//
// Created by mitchell on 7/9/20.
//
#include "socketaddress.hpp"
#include <regex>
#include <variant>
namespace sem::types {

namespace detail {
auto parse_port_from_string(const std::string& port) -> uint16_t
{
    long port_number = std::stoi(port);
    if(port_number > 65535 || port_number < 0) {
        throw std::invalid_argument("port given is out of range");
    }
    return static_cast<uint16_t>(port_number);
}
} // namespace detail

auto SocketAddress::parse_socket_address(const std::string& addr_string)
    -> std::pair<Ipv4, uint16_t>
{
    // Break out early if we're too long to avoid pathological regex case.
    if(addr_string.length() > 21) {
        throw std::invalid_argument("Invalid socket address string format; too long.");
    }

    const std::regex addr_check{R"--(^(.*):(\d{1,5})$)--"};
    std::smatch matches;
    if(!std::regex_match(addr_string.begin(), addr_string.end(), matches, addr_check)) {
        throw std::invalid_argument("Invalid socket address string format; cannot parse.");
    }
    try {
        return {Ipv4(matches[1].str()), detail::parse_port_from_string(matches[2].str())};
    } catch(const std::exception& ex) {
        throw std::invalid_argument(std::string("Invalid socket address string format; ")
                                    + ex.what());
    }
}
} // namespace sem::types