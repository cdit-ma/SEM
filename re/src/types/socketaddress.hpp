#ifndef CPP_UTIL_SOCKET_ADDR_HPP
#define CPP_UTIL_SOCKET_ADDR_HPP

#include "ipv4.hpp"
#include <cstdint>
#include <limits>
#include <variant>

namespace re::types {

/// Simple wrapper class for a re::types::Ipv4 + a port number
///  Note: No support for ipv6 addresses currently
class SocketAddress {
public:
    /// Construct Ipv4 addr from a string.
    /// Expected format 123.123.123.123:12345 or as a regex
    ///  (^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}:\d{1,5}$)
    [[deprecated("Use SocketAddress constructor instead.")]] [[nodiscard]] static auto
    from_string(const std::string& addr_string) -> SocketAddress
    {
        return SocketAddress(addr_string);
    }

    [[deprecated("Use SocketAddress constructor instead.")]] constexpr static auto
    from_ipv4(Ipv4 ip_addr, uint16_t port) -> SocketAddress
    {
        return SocketAddress(ip_addr, port);
    }

    constexpr auto set_ipv4(Ipv4 new_ip) -> void { ip_addr_ = new_ip; }

    constexpr auto set_port(uint16_t new_port) -> void { port_ = new_port; }

    [[nodiscard]] constexpr auto ip() const -> Ipv4 { return ip_addr_; }

    [[nodiscard]] constexpr auto port() const -> uint16_t { return port_; }

    [[nodiscard]] auto to_string() const -> std::string
    {
        return ip_addr_.to_string() + ':' + std::to_string(port_);
    }

    constexpr SocketAddress(Ipv4 ip_addr, uint16_t port) : ip_addr_(ip_addr), port_(port) {}

    SocketAddress(const std::string& addr_string) : SocketAddress(parse_socket_address(addr_string))
    {
    }

    explicit SocketAddress(const std::pair<Ipv4, uint16_t>& pair) :
        ip_addr_{pair.first}, port_{pair.second}
    {
    }

private:
    static auto parse_port_from_string(const std::string& port) -> uint16_t
    {
        long port_number = std::stoi(port);
        if(port_number > 65535 || port_number < 0) {
            throw std::invalid_argument("port given is out of range");
        }
        return static_cast<uint16_t>(port_number);
    }

    static auto parse_socket_address(const std::string& addr_string) -> std::pair<Ipv4, uint16_t>
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
            return {Ipv4(matches[1].str()), parse_port_from_string(matches[2].str())};
        } catch(const std::exception& ex) {
            throw std::invalid_argument(std::string("Invalid socket address string format; ")
                                        + ex.what());
        }
    }

    // TODO: Implement ipv6 support
    Ipv4 ip_addr_;
    uint16_t port_;
};

inline auto operator<<(std::ostream& out, const SocketAddress& addr) -> std::ostream&
{
    return out << addr.to_string();
}

inline auto operator==(const SocketAddress& a, const SocketAddress& b)
{
    return (a.port() == b.port()) && (a.ip() == b.ip());
}

} // namespace re::types
#endif // CPP_UTIL_SOCKET_ADDR_HPP