#include "gtest/gtest.h"
#include "../../../core/activatablefsmtester.h"

//Include the premade reqrep tests
#include "../../base/pubsubtests.hpp"

// Include the proto convert functions for the port type
#include "../../proto/basic.pb.h"

#include <core/ports/libportexport.h>
#include <middleware/zmq/pubsub/publisherport.hpp>
#include <middleware/zmq/pubsub/subscriberport.hpp>

bool setup_port(Port& port, const std::string& port_address){
    auto address = port.GetAttribute("publisher_address").lock();
    if(address){
        address->StringList().emplace_back("inproc://zmq" + port_address);
        return true;
    }
    return false;
}

void EmptyCallback(Base::Basic& m){
};

//Define an In/Out Port FSM Tester
class zmq_SubPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            component->RegisterCallback<void, Base::Basic>(port_name, EmptyCallback);
            auto port = ConstructSubscriberPort<zmq::SubscriberPort<Base::Basic, ::Basic> >(port_name, component);
            EXPECT_TRUE(setup_port(*port, {port_name}));
            a = port;
            ASSERT_TRUE(a);
        }
};

class zmq_PubPort_FSMTester : public ActivatableFSMTester{
protected:
    void SetUp(){
        ActivatableFSMTester::SetUp();
        auto port_name = get_long_test_name();
        auto port = ConstructPublisherPort<zmq::PublisherPort<Base::Basic, ::Basic> >(port_name, component);
        EXPECT_TRUE(setup_port(*port, {port_name}));
        a = port;
        ASSERT_TRUE(a);
    }
};

#define TEST_FSM_CLASS zmq_SubPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS zmq_PubPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(zmq_PubSub, Basic_Stable){
    using namespace ::PubSub::Basic::Stable;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = ::Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&rx_callback_count](base_type&){
        rx_callback_count ++;
    });

    auto c = std::make_shared<Component>("c_" + test_name);
    zmq::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    zmq::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    auto address = "inproc://" + test_name;
    //Setup Ports
    EXPECT_TRUE(setup_port(pub_port, address));
    EXPECT_TRUE(setup_port(sub_port, address));

    RunTest(pub_port, sub_port, rx_callback_count);
}

TEST(zmq_PubSub, Basic_Busy){
    using namespace ::PubSub::Basic::Busy;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = ::Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&rx_callback_count](base_type&){
        rx_callback_count ++;
        sleep_ms(1000);
    });

    auto c = std::make_shared<Component>("c_" + test_name);
    zmq::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    zmq::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    auto address = "inproc://" + test_name;
    //Setup Ports
    EXPECT_TRUE(setup_port(pub_port, address));
    EXPECT_TRUE(setup_port(sub_port, address));

    RunTest(pub_port, sub_port, rx_callback_count);
}

TEST(zmq_PubSub, Basic_Terminate){
    using namespace ::PubSub::Basic::Terminate;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = ::Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&rx_callback_count](base_type&){
        rx_callback_count ++;
    });

    auto c = std::make_shared<Component>("c_" + test_name);
    zmq::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    zmq::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    auto address = "inproc://" + test_name;
    //Setup Ports
    EXPECT_TRUE(setup_port(pub_port, address));
    EXPECT_TRUE(setup_port(sub_port, address));

    RunTest(pub_port, sub_port, rx_callback_count);
}