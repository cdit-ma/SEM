#include "gtest/gtest.h"

// Include the proto convert functions for the port type
#include "../../base/basic.h"
#include "../../proto/basic.pb.h"

#include <core/ports/libportexport.h>
#include <middleware/zmq/pubsub/publisherport.hpp>
#include <middleware/zmq/pubsub/subscriberport.hpp>

//Include the FSM Tester
#include "../../../core/activatablefsmtester.h"

void empty_callback(Base::Basic& b){};

bool setup_port(Port& port, const std::vector<std::string>& port_address){
    auto address = port.GetAttribute("publisher_address").lock();
    if(address){
        for(const auto& endpoint : port_address){
            address->StringList().emplace_back("inproc://zmq" + endpoint);
        }
        return true;
    }
    return false;
}


//Define an In/Out Port FSM Tester
class ZeroMQ_SubscriberPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            component->AddCallback<void, Base::Basic>(port_name, empty_callback);
            auto port = ConstructSubscriberPort<zmq::SubscriberPort<Base::Basic, ::Basic> >(port_name, component);
            EXPECT_TRUE(setup_port(*port, {port_name}));
            a = port;
            ASSERT_TRUE(a);
        }
};

class ZeroMQ_PublisherPort_FSMTester : public ActivatableFSMTester{
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


#define TEST_FSM_CLASS ZeroMQ_SubscriberPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS ZeroMQ_PublisherPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(ZeroMQ_EventportPair, Stable100){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    
    auto c = std::make_shared<Component>("Test");
    zmq::PublisherPort<Base::Basic, ::Basic> out_port(c, "tx_" + test_name);
    zmq::SubscriberPort<Base::Basic, ::Basic> in_port(c, "rx_" + test_name, [&rx_callback_count](Base::Basic&){
            rx_callback_count ++;
    });

    auto address = "inproc://" + test_name;
    {
        auto out_address = out_port.GetAttribute("publisher_address").lock();
        auto in_address = in_port.GetAttribute("publisher_address").lock();
        EXPECT_TRUE(out_address);
        EXPECT_TRUE(in_address);

        if(out_address && in_address){
            out_address->StringList().push_back(address);
            in_address->StringList().push_back(address);
        }
    }
    

    EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
       Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        out_port.Send(b);
        sleep_ms(1);
    }


    EXPECT_TRUE(in_port.Passivate());
    EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(in_port.Terminate());
    EXPECT_TRUE(out_port.Terminate());

    auto total_rxd = in_port.GetEventsReceived();
    auto total_received = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(total_received, send_count);
    EXPECT_EQ(rx_callback_count, send_count);
}

//Run a blocking callback which runs for 1 second,
//During that one second, send maximum num
TEST(ZeroMQ_EventportPair, Busy100){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    auto c = std::make_shared<Component>("Test");
    zmq::PublisherPort<Base::Basic, ::Basic> out_port(c, "tx_" + test_name);
    zmq::SubscriberPort<Base::Basic, ::Basic> in_port(c, "rx_" + test_name, [&rx_callback_count, &out_port](Base::Basic&){
            rx_callback_count ++;
            sleep_ms(1000);
    });

    auto address = "inproc://" + test_name;
    {
        auto out_address = out_port.GetAttribute("publisher_address").lock();
        auto in_address = in_port.GetAttribute("publisher_address").lock();
        EXPECT_TRUE(out_address);
        EXPECT_TRUE(in_address);

        if(out_address && in_address){
            out_address->StringList().push_back(address);
            in_address->StringList().push_back(address);
        }
    }


    EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        out_port.Send(b);
    }

    //Sleep for a reasonable time (Bigger than the callback work)
    sleep_ms(500);

    EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(in_port.Passivate());

    EXPECT_TRUE(out_port.Terminate());
    EXPECT_TRUE(in_port.Terminate());
    
    auto total_rxd = in_port.GetEventsReceived();
    auto total_received = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(total_received, 1);
    EXPECT_EQ(rx_callback_count, 1);
}


//Run a blocking callback which runs for 1 second,
//During that one second, send maximum num
TEST(ZeroMQ_SendAfterDead, Test1){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    auto c = std::make_shared<Component>("Test");
    zmq::PublisherPort<Base::Basic, ::Basic> out_port(c, "tx_" + test_name);
    

    auto address = "inproc://" + test_name;
    {
        auto out_address = out_port.GetAttribute("publisher_address").lock();
        EXPECT_TRUE(out_address);

        if(out_address){
            out_address->StringList().push_back(address);
        }
    }


    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(out_port.Activate());

    int send_count = 10;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        out_port.Send(b);
    }
    EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(out_port.Terminate());

    //Sleep for a reasonable time (Bigger than the callback work)
    sleep_ms(2000);


    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        out_port.Send(b);
    }


    

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count * 2);
    EXPECT_EQ(total_sent, send_count);
}

int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}