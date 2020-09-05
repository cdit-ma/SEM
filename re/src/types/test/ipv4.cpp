#include "gtest/gtest.h"
#include "ipv4.hpp"

namespace re::types::test {

TEST(re_types_ipv4, initialisation_test)
{
    constexpr auto test = Ipv4(0, 0, 0, 0);
    ASSERT_EQ(test, Ipv4::unspecified());
}

TEST(re_types_ipv4, equality_operator)
{
    constexpr auto a = Ipv4(123, 123, 123, 123);
    constexpr auto b = Ipv4(123, 123, 123, 123);
    constexpr auto c = Ipv4(123, 123, 123, 12);
    ASSERT_TRUE(a == b);
    ASSERT_FALSE(a == c);
}

TEST(re_types_ipv4, from_string)
{
    auto test = Ipv4("192.168.111.230");
    ASSERT_EQ(test, Ipv4(192, 168, 111, 230));

    auto short_test = Ipv4("1.1.1.1");
    ASSERT_EQ(short_test, Ipv4(1, 1, 1, 1));

    // Static cast to void to dodge [[nodiscard]] warnings.
    // Should fix this in the future by using a warning disable pragma.
    EXPECT_THROW(static_cast<void>(Ipv4("invalid_ipv4str")), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4("192.168.111.230.123")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4("300.168.111.230")), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4("200.168.111")), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4("192.168.111.2a1")), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4("   192.16.11.2")), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4("192..168.1")), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4("....")), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4(".168.111.230")), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4("aaa.aaa.aaa.aaa")), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(Ipv4("1 2.168.1 1.230")), std::invalid_argument);
}

TEST(re_types_ipv4, ip_constants)
{
    constexpr auto localhost = Ipv4::localhost();
    ASSERT_EQ(localhost, Ipv4(127, 0, 0, 1));

    constexpr auto broadcast = Ipv4::broadcast();
    ASSERT_EQ(broadcast, Ipv4(255, 255, 255, 255));

    constexpr auto unspecified = Ipv4::unspecified();
    ASSERT_EQ(unspecified, Ipv4(0, 0, 0, 0));
}

TEST(re_types_ipv4, ostream)
{
    constexpr auto unspecified = Ipv4::unspecified();
    std::ostringstream stream;
    stream << unspecified;
    ASSERT_EQ(stream.str(), "0.0.0.0");
}

TEST(re_types_ipv4, to_string)
{
    constexpr auto unspecified = Ipv4::unspecified();
    ASSERT_EQ(unspecified.to_string(), "0.0.0.0");

    std::string test_string{"192.168.111.230"};
    ASSERT_EQ(Ipv4(test_string).to_string(), test_string);
}

TEST(re_types_ipv4, copy_constructor)
{
    auto unspecified = Ipv4::unspecified();
    auto copy_assign = unspecified;
    ASSERT_EQ(unspecified, copy_assign);

    auto copy_construct{unspecified};
    ASSERT_EQ(unspecified, copy_construct);
}
} // namespace re::types::test
