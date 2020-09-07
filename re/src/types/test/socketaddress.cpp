#include "gtest/gtest.h"
#include "socketaddress.hpp"

namespace re::types::test {
TEST(re_types_socketaddress, equality_operator)
{
    constexpr auto a = SocketAddress({0, 0, 0, 0}, 0);
    constexpr auto b = SocketAddress(Ipv4(0, 0, 0, 0), 0);
    constexpr auto c = SocketAddress(Ipv4(0, 0, 0, 0), 10);
    constexpr auto d = SocketAddress(Ipv4(0, 0, 0, 0), 1);

    ASSERT_TRUE(a == b);
    ASSERT_FALSE(a == c);
    ASSERT_FALSE(c == d);
}

TEST(re_types_socketaddress, to_string)
{
    constexpr auto addr = SocketAddress(Ipv4(192, 168, 111, 230), 20000);

    ASSERT_EQ(addr.to_string(), "192.168.111.230:20000");
}

TEST(re_types_socketaddress, from_string)
{
    auto addr = SocketAddress("192.168.1.99:12345");
    ASSERT_EQ(addr.to_string(), "192.168.1.99:12345");

    EXPECT_THROW(SocketAddress("192.168.111.230:invalid_port_number_string"),
                 std::invalid_argument);
    EXPECT_THROW(SocketAddress("192.168.111.230:"),
                 std::invalid_argument);
    EXPECT_THROW(SocketAddress("192.168.111.230"),
                 std::invalid_argument);
    EXPECT_THROW(SocketAddress("192.168.111.230:1111111111"),
                 std::invalid_argument);
    EXPECT_THROW(SocketAddress("192.168.111.230:123invalid"),
                 std::invalid_argument);
    EXPECT_THROW(SocketAddress("192.168.111.230:123 4"),
                 std::invalid_argument);
}
} // namespace re::types::test
