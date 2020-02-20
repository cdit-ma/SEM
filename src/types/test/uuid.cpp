#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <types/uuid.h>

#include "gtest/gtest.h"

namespace re::types::test {
TEST(re_types_uuid, random_cons)
{
    auto uuid_a = Uuid{};
    auto uuid_b = Uuid{};
    EXPECT_NE(uuid_a, uuid_b);
}

TEST(re_types_uuid, string_cons)
{
    auto uuid_string = "00112233-4455-6677-8899-aabbccddeeff";
    auto uuid = types::Uuid{uuid_string};
    EXPECT_EQ(uuid, types::Uuid{uuid_string});
}

TEST(re_types_uuid, boost_uuid_cons)
{
    boost::uuids::random_generator generator;
    auto boost_uuid = generator();
    Uuid re_uuid{boost_uuid};

    EXPECT_EQ(boost::uuids::to_string(boost_uuid), re_uuid.to_string());
}

TEST(re_types_uuid, uuid_literal_cons)
{
    auto uuid = "00112233-4455-6677-8899-aabbccddeeff"_uuid;
    EXPECT_EQ(types::Uuid{"00112233-4455-6677-8899-aabbccddeeff"}, uuid);
}

TEST(re_types_uuid, uuid_hash_function)
{
    auto a = "00112233-4455-6677-8899-aabbccddeeff"_uuid;
    EXPECT_EQ(a.hash(), 18350266262164909475u);

    auto b = "00000000-0000-0000-0000-000000000000"_uuid;
    EXPECT_EQ(b.hash(), 17562157925649023279u);
}

TEST(re_types_uuid, unordered_map_key)
{
    std::unordered_map<Uuid, std::string> map;
    auto pair = std::make_pair(Uuid{}, "test_string");
    map.insert(pair);

    EXPECT_EQ(map.at(pair.first), "test_string");
}
} // namespace re::types::test
