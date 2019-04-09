#include "gtest/gtest.h"
#include "../../../core/activatablefsmtester.h"

//Include the premade reqrep tests
#include "../../base/pubsubtests.hpp"

// Include the proto convert functions for the port type
#include "basic.pb.h"

#include <core/ports/libportexport.h>
#include <middleware/qpid/pubsub/publisherport.hpp>
#include <middleware/qpid/pubsub/subscriberport.hpp>

//Include main and env passing magic
#include "../../../core/test_main.h"

bool setup_port(Port& port, const std::string& topic_name){
    auto b = port.GetAttribute("broker").lock();
    auto t = port.GetAttribute("topic_name").lock();
   
    if(b && t){
        b->set_String(cditma::GetQpidBrokerAddress());
        t->set_String(cditma::GetNodeName() + "_" + topic_name);
        return true;
    }
    return false;
}

void EmptyCallback(Base::Basic& m){
};

//Define an In/Out Port FSM Tester
class Re_Port_Sub_Qpid_FSM : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            component->RegisterCallback<void, Base::Basic>(port_name, EmptyCallback);
            auto port = ConstructSubscriberPort<qpid::SubscriberPort<Base::Basic, ::Basic> >(port_name, component);
            EXPECT_TRUE(setup_port(*port, port_name));
            a = port;
            ASSERT_TRUE(a);
        }
};

class Re_Port_Pub_Qpid_FSM : public ActivatableFSMTester{
protected:
    void SetUp(){
        ActivatableFSMTester::SetUp();
        auto port_name = get_long_test_name();
        auto port = ConstructPublisherPort<qpid::PublisherPort<Base::Basic, ::Basic> >(port_name, component);
        EXPECT_TRUE(setup_port(*port, port_name));
        a = port;
        ASSERT_TRUE(a);
    }
};

#define TEST_FSM_CLASS Re_Port_Sub_Qpid_FSM
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS Re_Port_Pub_Qpid_FSM
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(Re_Port_PubSub_Qpid, Basic_Stable){
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
    qpid::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    qpid::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    //Setup Ports
    EXPECT_TRUE(setup_port(pub_port, test_name));
    EXPECT_TRUE(setup_port(sub_port, test_name));

    RunTest(pub_port, sub_port, rx_callback_count);
}

TEST(Re_Port_PubSub_Qpid, Basic_Busy){
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
    qpid::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    qpid::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    //Setup Ports
    EXPECT_TRUE(setup_port(pub_port, test_name));
    EXPECT_TRUE(setup_port(sub_port, test_name));

    RunTest(pub_port, sub_port, rx_callback_count);
}

TEST(Re_Port_PubSub_Qpid, Basic_Terminate){
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
    qpid::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    qpid::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    //Setup Ports
    EXPECT_TRUE(setup_port(pub_port, test_name));
    EXPECT_TRUE(setup_port(sub_port, test_name));

    RunTest(pub_port, sub_port, rx_callback_count);
}