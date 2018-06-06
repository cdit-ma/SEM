#include "gtest/gtest.h"

// Include the proto convert functions for the port type
#include "../../base/basic.h"
#include "../../proto/basic.pb.h"

#include <middleware/zmq/requestreply/replierport.hpp>
#include <middleware/zmq/requestreply/requesterport.hpp>

//Include the FSM Tester
#include "../../../core/activatablefsmtester.h"

void empty_callback(Base::Basic& b){};

Base::Basic callback(Base::Basic& message){
    //std::cout << "REPLIER: GOT MESSAGE: " << std::endl;
    //std::cout << "Int: " << message.int_val << std::endl;
    //std::cout << "str_val: " << message.str_val << std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(message.int_val * 10));
    message.int_val *= 10;
    return message;
}

Base::Basic busy_callback(Base::Basic& message){
    //std::cout << "REPLIER: GOT MESSAGE: " << std::endl;
    //std::cout << "Int: " << message.int_val << std::endl;
    //std::cout << "str_val: " << message.str_val << std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(message.int_val * 10));
    message.int_val *= 10;
    sleep_ms(50);
    return message;
}

bool setup_port(Port& port, std::string port_name){
    auto address = port.GetAttribute("server_address").lock();
    if(address){
        address->set_String("inproc://zmq" + port_name);
        return true;
    }
    return false;
}

//Define an In/Out Port FSM Tester
class ZMQ_RequesterPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = new zmq::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic>(std::weak_ptr<Component>(),  port_name);
            EXPECT_TRUE(setup_port(*port, port_name));
            a = port;
            ASSERT_TRUE(a);
        }
};

class ZMQ_ReplierPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = new zmq::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic>(std::weak_ptr<Component>(), port_name, callback);
            EXPECT_TRUE(setup_port(*port, port_name));
            a = port;
            ASSERT_TRUE(a);
        }
};


// #define TEST_FSM_CLASS ZMQ_RequesterPort_FSMTester
// #include "../../../core/activatablefsmtestcases.h"
// #undef TEST_FSM_CLASS

// #define TEST_FSM_CLASS ZMQ_ReplierPort_FSMTester
// #include "../../../core/activatablefsmtestcases.h"
// #undef TEST_FSM_CLASS

TEST(ZMQ_EventportPair, Stable100){
    std::string test_name = get_long_test_name();

    
    auto c = std::make_shared<Component>("Test");
    zmq::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic> out_port(c, test_name);
    zmq::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic> in_port(c, test_name, callback);

    EXPECT_TRUE(setup_port(out_port, test_name));
    EXPECT_TRUE(setup_port(in_port, test_name));
    

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
        auto rep = out_port.ProcessRequest(b, std::chrono::milliseconds(10));
        sleep_ms(1);
    }


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
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
}

//Run a blocking callback which runs for 1 second,
//During that one second, send maximum num
TEST(ZMQ_EventportPair, Busy100){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    auto c = std::make_shared<Component>("Test");
    zmq::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic> out_port(c, "tx_" + test_name);
    zmq::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic> in_port(c, "rx_" + test_name, busy_callback);

    
    EXPECT_TRUE(setup_port(in_port, test_name));
    EXPECT_TRUE(setup_port(out_port, test_name));

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
        out_port.ProcessRequest(b, std::chrono::milliseconds(100));
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
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
}

int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}