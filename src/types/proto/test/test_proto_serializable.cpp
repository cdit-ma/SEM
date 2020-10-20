#include "proto_serializable.hpp"
#include "testmessage.pb.h"
#include <iostream>

#include "gtest/gtest.h"

namespace sem::types::proto::test {

TEST(sem_types_serializable, protobuf_test)
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

} // namespace sem::types::proto::test