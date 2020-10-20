#include "network/replier.hpp"
#include "network/requester.hpp"
#include "gtest/gtest.h"
#include "modelevent.pb.h"

namespace re::network::test {

constexpr int default_broker_port{5672};

constexpr auto broker_address = types::SocketAddress::from_ipv4(types::Ipv4::localhost(),
                                                                default_broker_port);
// TODO: Add more tests that cover more than simple case.
// TODO: Fix tests to work on more than one machine.
// TODO: Add protobuf type to use for these tests.
TEST(re_network_req_rep, simple_test)
{
//    Requester<ModelEvent::Component, ModelEvent::Component> requester{broker_address, "test_topic",
//                                                                      "test_subject"};
//    Replier<ModelEvent::Component, ModelEvent::Component> replier{broker_address, "test_topic",
//                                                                  "test_subject"};
//
//    replier.run([](ModelEvent::Component message) { return message; });
//    ModelEvent::Component comp;
//    comp.set_type("asdf");
//    auto reply = requester.request(comp, re::types::NeverTimeout());
//
//    // Add second request to make sure we don't repeat the mistakes of our past
//    auto reply2 = requester.request(comp, re::types::NeverTimeout());
//
//    replier.stop();
//
//    ASSERT_EQ(comp.DebugString(), reply.DebugString());
//    ASSERT_EQ(comp.DebugString(), reply2.DebugString());
}

} // namespace re::network::test
