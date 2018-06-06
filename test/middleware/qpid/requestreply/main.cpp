#include "gtest/gtest.h"

// Include the proto convert functions for the port type
#include "../../base/basic.h"
#include "../../proto/basic.pb.h"

#include <middleware/qpid/requestreply/replierport.hpp>
#include <middleware/qpid/requestreply/requesterport.hpp>

#include <core/ports/libportexport.h>
//Include the FSM Tester
#include "../../../core/activatablefsmtester.h"


void empty_callback(Base::Basic& b){};

std::string broker("127.0.0.1:5672");

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
    sleep_ms(100);
    return message;
}

bool setup_port(Port& port, std::string topic_name){
    auto b = port.GetAttribute("broker").lock();
    auto t = port.GetAttribute("topic_name").lock();
   
    if(b && t){
        b->set_String(broker);
        t->set_String(topic_name);
        return true;
    }
    return false;
}

//Define an In/Out Port FSM Tester
class QPID_RequesterPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = new qpid::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic>(std::weak_ptr<Component>(),  port_name);
            EXPECT_TRUE(setup_port(*port, port_name));
            a = port;
            ASSERT_TRUE(a);
        }
};

class QPID_ReplierPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = new qpid::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic>(std::weak_ptr<Component>(), port_name, callback);
            EXPECT_TRUE(setup_port(*port, port_name));
            a = port;
            ASSERT_TRUE(a);
        }
};


// #define TEST_FSM_CLASS QPID_RequesterPort_FSMTester
// #include "../../../core/activatablefsmtestcases.h"
// #undef TEST_FSM_CLASS

// #define TEST_FSM_CLASS QPID_ReplierPort_FSMTester
// #include "../../../core/activatablefsmtestcases.h"
// #undef TEST_FSM_CLASS

TEST(QPID_EventportPair, Stable100){
    std::string test_name = get_long_test_name();

    
    auto c = std::make_shared<Component>("Test");
    qpid::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic> out_port(c, test_name);
    qpid::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic> in_port(c, test_name, callback);

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
        auto rep = out_port.ProcessRequest(b, std::chrono::milliseconds(20));
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
TEST(QPID_EventportPair, Busy100){
    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>(test_name);
    component->AddCallback<Base::Basic, Base::Basic>(rep_name, new CallbackWrapper<Base::Basic, Base::Basic>(busy_callback));

    auto requester_port = ConstructRequesterPort<qpid::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic>>(req_name, component);
    auto replier_port = ConstructReplierPort<qpid::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic>>(rep_name, component);
    
    EXPECT_TRUE(setup_port(*requester_port, test_name));
    EXPECT_TRUE(setup_port(*replier_port, test_name));

    EXPECT_TRUE(requester_port->Configure());
    EXPECT_TRUE(replier_port->Configure());
    EXPECT_TRUE(requester_port->Activate());
    EXPECT_TRUE(replier_port->Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        auto c = requester_port->SendRequest(b, std::chrono::milliseconds(200));
        if(c.first){
            std::cerr << c.second.int_val << std::endl;
            std::cerr << c.second.str_val << std::endl;
        }
    }

    //Sleep for a reasonable time (Bigger than the callback work)
    sleep_ms(500);

    EXPECT_TRUE(requester_port->Passivate());
    EXPECT_TRUE(replier_port->Passivate());

    EXPECT_TRUE(requester_port->Terminate());
    EXPECT_TRUE(replier_port->Terminate());

    auto total_rxd = replier_port->GetEventsReceived();
    auto proc_rxd = replier_port->GetEventsProcessed();

    auto total_txd = requester_port->GetEventsReceived();
    auto total_sent = requester_port->GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
    delete requester_port;
    delete replier_port;
}

int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}