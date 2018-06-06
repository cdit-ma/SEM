#include "gtest/gtest.h"

// Include the proto convert functions for the port type
#include "../../base/basic.h"
#include "../../proto/basic.pb.h"

#include <core/ports/libportexport.h>
#include <middleware/qpid/requestreply/replierport.hpp>
#include <middleware/qpid/requestreply/requesterport.hpp>

//Include the FSM Tester
#include "../../../core/activatablefsmtester.h"


void empty_callback(Base::Basic& b){};

const std::string broker("127.0.0.1:5672");

Base::Basic callback(Base::Basic& message){
    message.int_val *= 10;
    return message;
}

Base::Basic busy_callback(Base::Basic& message){
    message.int_val *= 10;
    sleep_ms(100);
    return message;
}


void busy_callback_no_reply(Base::Basic& message){
    sleep_ms(100);
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


#define TEST_FSM_CLASS QPID_RequesterPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS QPID_ReplierPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(QPID_ReqRep, Req_Basic_Rep_Basic_Busy100){
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
        EXPECT_TRUE(c.first);
    }
    
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

TEST(QPID_ReqRep, Req_Basic_Rep_Void_Busy100){
    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>(test_name);
    component->AddCallback<void, Base::Basic>(rep_name, new CallbackWrapper<void, Base::Basic>(busy_callback_no_reply));

    auto requester_port = ConstructRequesterPort<qpid::RequesterPort<void, void, Base::Basic, ::Basic>>(req_name, component);
    auto replier_port = ConstructReplierPort<qpid::ReplierPort<void, void, Base::Basic, ::Basic>>(rep_name, component);
    
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
        EXPECT_TRUE(c);
    }
    
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