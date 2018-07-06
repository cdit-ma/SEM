#include "gtest/gtest.h"
#include "../../../core/activatablefsmtester.h"

//Include the premade reqrep tests
#include "../../base/pubsubtests.hpp"

// Include the proto convert functions for the port type
#include "basic_DCPS.hpp"

#include <core/ports/libportexport.h>
#include <middleware/ospl/pubsub/publisherport.hpp>
#include <middleware/ospl/pubsub/subscriberport.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

boost::uuids::random_generator uuid_generator;

bool setup_port(Port& port, const std::string& topic_name){
    auto d = port.GetAttribute("domain_id").lock();
    auto t = port.GetAttribute("topic_name").lock();
   
    if(d && t){
        d->set_Integer(0);
        t->set_String(topic_name);
        return true;
    }
    return false;
};

void EmptyCallback(Base::Basic& m){
};

//Define an In/Out Port FSM Tester
class ospl_SubPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            component->RegisterCallback<void, Base::Basic>(port_name, EmptyCallback);
            auto port = ConstructSubscriberPort<ospl::SubscriberPort<Base::Basic, Basic> >(port_name, component);
            EXPECT_TRUE(setup_port(*port, port_name));
            a = port;
            ASSERT_TRUE(a);
        }
};

class ospl_PubPort_FSMTester : public ActivatableFSMTester{
protected:
    void SetUp(){
        ActivatableFSMTester::SetUp();
        auto port_name = get_long_test_name();
        auto port = ConstructPublisherPort<ospl::PublisherPort<Base::Basic, Basic> >(port_name, component);
        EXPECT_TRUE(setup_port(*port, port_name));
        a = port;
        ASSERT_TRUE(a);
    }
};

#define TEST_FSM_CLASS ospl_SubPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS ospl_PubPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(ospl_PubSub, Basic_Stable){
    using namespace ::PubSub::Basic::Stable;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&rx_callback_count](base_type& m){
        rx_callback_count ++;
    });

    auto c = std::make_shared<Component>("c_" + test_name);
    ospl::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    ospl::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    
    auto topic_name = boost::uuids::to_string(uuid_generator()); 
    //Setup Ports
    EXPECT_TRUE(setup_port(pub_port, topic_name));
    EXPECT_TRUE(setup_port(sub_port, topic_name));

    RunTest(pub_port, sub_port, rx_callback_count);
}

TEST(ospl_PubSub, Basic_Busy){
    using namespace ::PubSub::Basic::Busy;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&rx_callback_count](base_type&){
        rx_callback_count ++;
        sleep_ms(1000);
    });

    auto c = std::make_shared<Component>("c_" + test_name);
    ospl::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    ospl::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    
    auto topic_name = boost::uuids::to_string(uuid_generator()); 
    //Setup Ports
    EXPECT_TRUE(setup_port(pub_port, topic_name));
    EXPECT_TRUE(setup_port(sub_port, topic_name));

    RunTest(pub_port, sub_port, rx_callback_count);
}

TEST(ospl_PubSub, Basic_Terminate){
    using namespace ::PubSub::Basic::Terminate;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&rx_callback_count](base_type&){
        rx_callback_count ++;
    });

    auto c = std::make_shared<Component>("c_" + test_name);
    ospl::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    ospl::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    
    auto topic_name = boost::uuids::to_string(uuid_generator()); 
    //Setup Ports
    EXPECT_TRUE(setup_port(pub_port, topic_name));
    EXPECT_TRUE(setup_port(sub_port, topic_name));

    RunTest(pub_port, sub_port, rx_callback_count);
}