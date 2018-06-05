#include "gtest/gtest.h"

#include "../base/basic.h"
#include "basic_DCPS.hpp"

#include <middleware/ospl/pubsub/subscriberport.hpp>
#include <middleware/ospl/pubsub/publisherport.hpp>

//Include the FSM Tester
#include "../../core/activatablefsmtester.h"

#include <algorithm>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


void empty_callback(Base::Basic& b){};

bool setup_port(Port& port, int domain, std::string topic_name){
    auto d = port.GetAttribute("domain_id").lock();
    auto t = port.GetAttribute("topic_name").lock();
   
    if(d && t){
        d->set_Integer(domain);
        t->set_String(topic_name);
        return true;
    }
    return false;
}

//Define an In/Out Port FSM Tester
class OSPL_SubscriberPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = new ospl::SubscriberPort<Base::Basic, Basic>(std::weak_ptr<Component>(),  port_name, empty_callback);
            
            EXPECT_TRUE(setup_port(*port, 0, port_name));

            a = port;
            ASSERT_TRUE(a);
        }
};

class OSPL_PublisherPort_FSMTester : public ActivatableFSMTester{
protected:
    void SetUp(){
        ActivatableFSMTester::SetUp();
        auto port_name = get_long_test_name();
        auto port = new ospl::PublisherPort<Base::Basic, Basic>(std::weak_ptr<Component>(), port_name);
        EXPECT_TRUE(setup_port(*port, 0, port_name));
        a = port;
        ASSERT_TRUE(a);
    }
};


#define TEST_FSM_CLASS OSPL_SubscriberPort_FSMTester
#include "../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS OSPL_PublisherPort_FSMTester
#include "../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(OSPL_EventportPair, Stable100){
    auto test_name = get_long_test_name();
    int send_count = 100;

    //Set up guid matching structures
    boost::uuids::random_generator uuid_generator_;
    std::set<std::string> guids;
    for(int i = 0; i < send_count; i++){
        guids.insert(boost::uuids::to_string(uuid_generator_()));
    }
    std::set<std::string> expected_guids = guids;

    auto c = std::make_shared<Component>("Test");
    ospl::PublisherPort<Base::Basic, Basic> out_port(c, "tx_" + test_name);
    ospl::SubscriberPort<Base::Basic, Basic> in_port(c, "rx_" + test_name, [&expected_guids](Base::Basic& message){
        expected_guids.erase(message.guid_val);
    });

    EXPECT_TRUE(setup_port(out_port, 0, test_name));
    EXPECT_TRUE(setup_port(in_port, 0, test_name));

    EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());

    int counter = 0;
    //Send as fast as possible
    for(auto &guid : guids){
        Base::Basic b;
        b.int_val = counter++;
        b.str_val = std::to_string(b.int_val);
        b.guid_val = guid;
        out_port.Send(b);
    }

    sleep_ms(100);

    EXPECT_TRUE(in_port.Passivate());
    EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(in_port.Terminate());
    EXPECT_TRUE(out_port.Terminate());

    auto total_rxd = in_port.GetEventsReceived();
    auto proc_rxd = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(expected_guids.size(), 0);
    EXPECT_GE(total_rxd, send_count);
    EXPECT_GE(proc_rxd, send_count);

}

//Run a blocking callback which runs for 1 second,
//During that one second, send maximum num
TEST(OSPL_EventportPair, Busy100){
    auto test_name = get_long_test_name();
    int send_count = 100;

    //Set up guid matching structures
    boost::uuids::random_generator uuid_generator_;
    std::set<std::string> guids;
    for(int i = 0; i < send_count; i++){
        guids.insert(boost::uuids::to_string(uuid_generator_()));
    }
    std::set<std::string> expected_guids = guids;

    auto c = std::make_shared<Component>("Test");
    ospl::PublisherPort<Base::Basic, Basic> out_port(c, "tx_" + test_name);
    ospl::SubscriberPort<Base::Basic, Basic> in_port(c, "rx_" + test_name, [&expected_guids](Base::Basic& message){
        //Only sleep on messages we're meant to receive
        if(expected_guids.erase(message.guid_val)){
            sleep_ms(2000);
        }
    });
    
    EXPECT_TRUE(setup_port(out_port, 0, test_name));
    EXPECT_TRUE(setup_port(in_port, 0, test_name));

    EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());

    int counter = 0;
    //Send as fast as possible
    for(auto &guid : guids){
        Base::Basic b;
        b.int_val = counter++;
        b.str_val = std::to_string(b.int_val);
        b.guid_val = guid;
        out_port.Send(b);
        sleep_ms(1);
    }

    //Sleep for a reasonable time (Bigger than the callback work)
    sleep_ms(500);

    EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(in_port.Passivate());

    EXPECT_TRUE(out_port.Terminate());
    EXPECT_TRUE(in_port.Terminate());

    auto total_rxd = in_port.GetEventsReceived();
    auto proc_rxd = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(expected_guids.size(), guids.size() - 1);
    EXPECT_GE(total_rxd, send_count);
    EXPECT_GE(proc_rxd, 1);
}

int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}