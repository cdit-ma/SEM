#ifndef CPP_UTIL_IPV4_HPP
#define CPP_UTIL_IPV4_HPP
#include <array>
#include <string>
#include <ostream>

namespace re::types {

class Ipv4 {
public:
    [[deprecated("Use Ipv4 constructor instead.")]] [[nodiscard]] static auto
    from_string(const std::string& addr_string) -> Ipv4
    {
        return Ipv4(addr_string);
    }

    [[deprecated("Use Ipv4 constructor instead.")]] [[nodiscard]] constexpr static auto
    from_ints(const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d) -> Ipv4
    {
        return Ipv4(a, b, c, d);
    };

    [[deprecated("Use Ipv4 constructor instead.")]] [[nodiscard]] constexpr static auto
    from_array(const std::array<uint8_t, 4>& array) -> Ipv4
    {
        return Ipv4(array);
    }

    [[nodiscard]] constexpr static auto unspecified() -> Ipv4 { return Ipv4(0, 0, 0, 0); }

    [[nodiscard]] constexpr static auto broadcast() -> Ipv4 { return Ipv4(255, 255, 255, 255); }

    [[nodiscard]] constexpr static auto localhost() -> Ipv4 { return Ipv4(127, 0, 0, 1); }

    [[nodiscard]] constexpr auto octets() const -> std::array<uint8_t, 4> { return addr_; }

    [[nodiscard]] auto to_string() const -> std::string
    {
        // TODO: Profile this
        //  then
        // TODO: Optimise this...
        return std::string{std::to_string(addr_[0]) + '.' + std::to_string(addr_[1]) + '.'
                           + std::to_string(addr_[2]) + '.' + std::to_string(addr_[3])};
    }

    constexpr Ipv4(const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d) :
        addr_{a, b, c, d}
    {
    }

    constexpr explicit Ipv4(const std::array<uint8_t, 4>& array) :
        Ipv4(array[0], array[1], array[2], array[3])
    {
    }

    /// Construct Ipv4 addr from a string.
    /// Expected format 123.123.123.123 or as a regex (^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$)
    explicit Ipv4(const std::string& addr_string) : addr_{parse_string_to_octets(addr_string)} {}

    /// Copy/move constructor
    constexpr Ipv4(const Ipv4&) = default;
    constexpr Ipv4(Ipv4&&) = default;

    /// Copy/move assignment operator
    constexpr auto operator=(const Ipv4&) -> Ipv4& = default;
    constexpr auto operator=(Ipv4&&) -> Ipv4& = default;

private:
    std::array<uint8_t, 4> addr_;
    [[nodiscard]] static auto
    parse_string_to_octets(const std::string& addr_string) -> std::array<uint8_t, 4>;
};

inline auto operator<<(std::ostream& out, const Ipv4& addr) -> std::ostream&
{
    return out << addr.to_string();
}
inline auto operator==(const Ipv4& a, const Ipv4& b)
{
    return a.octets() == b.octets();
}

} // namespace re::types

#endif // CPP_UTIL_IPV4_HPP
