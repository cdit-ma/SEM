#include "gtest/gtest.h"

// Include the proto convert functions for the port type
#include "convert.hpp"
#include <middleware/zmq/ineventport.hpp>
#include <middleware/zmq/outeventport.hpp>

//Include the FSM Tester
#include "../../core/activatablefsmtester.h"

void empty_callback(Base::Basic* b){};

//Define an In/Out Port FSM Tester
class ZeroMQ_InEventPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = new zmq::InEventPort<Base::Basic, Basic>(0,  port_name, empty_callback);
            port->GetAttribute("publisher_address")->StringList().push_back("inproc://zmq" + port_name);
            a = port;
            ASSERT_TRUE(a);
        }
};

class ZeroMQ_OutEventPort_FSMTester : public ActivatableFSMTester{
protected:
    void SetUp(){
        ActivatableFSMTester::SetUp();
        auto port_name = get_long_test_name();
        auto port = new zmq::OutEventPort<Base::Basic, Basic>(0, port_name);
        port->GetAttribute("publisher_address")->StringList().push_back("inproc://zmq" + port_name);
        a = port;
        ASSERT_TRUE(a);
    }
};

#define TEST_FSM_CLASS ZeroMQ_InEventPort_FSMTester
#include "../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS ZeroMQ_OutEventPort_FSMTester
#include "../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS


TEST(ZeroMQ_EventportPair, Stable100){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    zmq::OutEventPort<Base::Basic, Basic> out_port(nullptr, "tx_" + test_name);
    zmq::InEventPort<Base::Basic, Basic> in_port(nullptr, "rx_" + test_name, [&rx_callback_count](Base::Basic*){
            rx_callback_count ++;
    });

    auto address = "inproc://" + get_long_test_name();
    out_port.GetAttribute("publisher_address")->StringList().push_back(address);
    in_port.GetAttribute("publisher_address")->StringList().push_back(address);

    EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        auto b = new Base::Basic();
        b->int_val = i;
        b->str_val = std::to_string(i);
        out_port.tx(b);
        sleep_ms(1);
    }


    EXPECT_TRUE(in_port.Passivate());
    EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(in_port.Terminate());
    EXPECT_TRUE(out_port.Terminate());

    auto total_rxd = in_port.GetEventsReceieved();
    auto proc_rxd = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsSent();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
    EXPECT_EQ(rx_callback_count, send_count);
}

//Run a blocking callback which runs for 1 second,
//During that one second, send maximum num
TEST(ZeroMQ_EventportPair, Busy100){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    zmq::OutEventPort<Base::Basic, Basic> out_port(nullptr, "tx_" + test_name);
    zmq::InEventPort<Base::Basic, Basic> in_port(nullptr, "rx_" + test_name, [&rx_callback_count, &out_port](Base::Basic*){
            rx_callback_count ++;
            sleep_ms(100);
    });

    auto address = "inproc://" + get_long_test_name();
    out_port.GetAttribute("publisher_address")->StringList().push_back(address);
    in_port.GetAttribute("publisher_address")->StringList().push_back(address);

    EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        auto b = new Base::Basic();
        b->int_val = i;
        b->str_val = std::to_string(i);
        out_port.tx(b);
    }

    EXPECT_TRUE(in_port.Passivate());
    EXPECT_TRUE(out_port.Passivate());

    EXPECT_TRUE(in_port.Terminate());
    EXPECT_TRUE(out_port.Terminate());

    auto total_rxd = in_port.GetEventsReceieved();
    auto proc_rxd = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsSent();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, 1);
    EXPECT_EQ(rx_callback_count, 1);
}

int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}