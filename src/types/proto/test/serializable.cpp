#include "testprotomessage/testmessage.pb.h"
#include <iostream>
#include <types/proto/serializable.hpp>

#include "gtest/gtest.h"

namespace re::types::proto::test {

TEST(re_types_serializable, protobuf_test)
{
    Message test_message;
    test_message.set_string("string");
    test_message.set_number(10);
    test_message.set_signed_number(-10);
    test_message.add_repeated_numbers(10);
    test_message.set_enum_(Message::ENUM_OPT_A);

    Serializable<Message> serializable{test_message};
    std::string serialized{serializable.serialize()};

    Message out;
    out = Serializable<Message>::deserialize(serialized);
    EXPECT_EQ(out.string(), "string");
    EXPECT_EQ(out.number(), 10);
    EXPECT_EQ(out.signed_number(), -10);
}

} // namespace re::types::proto::test