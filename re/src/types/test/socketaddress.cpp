#include "gtest/gtest.h"
#include "socketaddress.hpp"

namespace re::types::test {
TEST(re_types_socketaddress, equality_operator)
{
    constexpr auto a = SocketAddress::from_ipv4(Ipv4::from_ints(0, 0, 0, 0), 0);
    constexpr auto b = SocketAddress::from_ipv4(Ipv4::from_ints(0, 0, 0, 0), 0);
    constexpr auto c = SocketAddress::from_ipv4(Ipv4::from_ints(0, 0, 0, 0), 10);
    constexpr auto d = SocketAddress::from_ipv4(Ipv4::from_ints(0, 0, 0, 0), 1);

    ASSERT_TRUE(a == b);
    ASSERT_FALSE(a == c);
    ASSERT_FALSE(c == d);
}

TEST(re_types_socketaddress, to_string)
{
    constexpr auto addr = SocketAddress::from_ipv4(Ipv4::from_ints(192, 168, 111, 230), 20000);

    ASSERT_EQ(addr.to_string(), "192.168.111.230:20000");
}

TEST(re_types_socketaddress, from_string)
{
    auto addr = SocketAddress::from_string("192.168.1.99:12345");
    ASSERT_EQ(addr.to_string(), "192.168.1.99:12345");

    // Static cast to void to dodge [[nodiscard]] warnings....
    // Seems terrible, couldn't find warning number in any documentation, couldn't use pragma
    // warning disable.
    EXPECT_THROW(static_cast<void>(SocketAddress::from_string("192.168.111.230:adsf")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(SocketAddress::from_string("192.168.111.230:")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(SocketAddress::from_string("192.168.111.230")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(SocketAddress::from_string("192.168.111.230:1111111111")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(SocketAddress::from_string("192.168.111.230:123adsf")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(SocketAddress::from_string("192.168.111.230:123 4")),
                 std::invalid_argument);
}
} // namespace re::types::test
