#include "gtest/gtest.h"
#include "../../../core/activatablefsmtester.h"

//Include the premade reqrep tests
#include "../../base/reqreptests.hpp"

// Include the proto convert functions for the port type
#include "../../proto/basic.pb.h"

#include <core/ports/libportexport.h>
#include <middleware/zmq/requestreply/replierport.hpp>
#include <middleware/zmq/requestreply/requesterport.hpp>



bool setup_port(Port& port, int test_id){
    auto address = port.GetAttribute("server_address").lock();
    if(address){
        std::string port_address = "tcp://127.0.0.1:" + std::to_string(7000 + test_id);
        address->set_String(port_address);
        return true;
    }
    return false;
}

Base::Basic EmptyCallback(Base::Basic& m){
    return m;
};

//Define an Requester Port FSM Tester
class ZMQ_RequesterPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = ConstructRequesterPort<zmq::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic>>(port_name, component);
            EXPECT_TRUE(setup_port(*port, 0));
            a = port;
            ASSERT_TRUE(a);
        }
};

//Define an Replier Port FSM Tester
class ZMQ_ReplierPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            component->RegisterCallback<Base::Basic, Base::Basic>(port_name, EmptyCallback);
            auto port = ConstructReplierPort<zmq::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic>>(port_name, component);
            EXPECT_TRUE(setup_port(*port, 0));
            a = port;
            ASSERT_TRUE(a);
        }
};

#define TEST_FSM_CLASS ZMQ_RequesterPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS ZMQ_ReplierPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(zmq_ReqRep, Basic2Basic_Stable){
    using namespace ::ReqRep::Basic2Basic::Stable;

    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_reply_type = ::Basic;
    using mw_request_type = ::Basic;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);
    auto requester_port = ConstructRequesterPort<zmq::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<zmq::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_port(*requester_port, 0));
    EXPECT_TRUE(setup_port(*replier_port, 0));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(zmq_ReqRep, LONG_Basic2Basic_Busy){
    using namespace ::ReqRep::Basic2Basic::Busy;

    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_reply_type = ::Basic;
    using mw_request_type = ::Basic;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);
    auto requester_port = ConstructRequesterPort<zmq::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<zmq::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_port(*requester_port, 0));
    EXPECT_TRUE(setup_port(*replier_port, 0));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(zmq_ReqRep, LONG_Basic2Basic_Timeout){
    using namespace ::ReqRep::Basic2Basic::Timeout;
    
    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_reply_type = ::Basic;
    using mw_request_type = ::Basic;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);
    auto requester_port = ConstructRequesterPort<zmq::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<zmq::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_port(*requester_port, 0));
    EXPECT_TRUE(setup_port(*replier_port, 0));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(zmq_ReqRep, Basic2Void_Stable){
    using namespace ::ReqRep::Basic2Void::Stable;
    
    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = void;
    
    //Define the proto types
    using mw_request_type = ::Basic;
    using mw_reply_type = void;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);

    auto requester_port = ConstructRequesterPort<zmq::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<zmq::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_port(*requester_port, 0));
    EXPECT_TRUE(setup_port(*replier_port, 0));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(zmq_ReqRep, LONG_Basic2Void_Busy){
    using namespace ::ReqRep::Basic2Void::Busy;
    
    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = void;
    
    //Define the proto types
    using mw_request_type = ::Basic;
    using mw_reply_type = void;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);

    auto requester_port = ConstructRequesterPort<zmq::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<zmq::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_port(*requester_port, 0));
    EXPECT_TRUE(setup_port(*replier_port, 0));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(zmq_ReqRep, Void2Basic_Stable){
    using namespace ::ReqRep::Void2Basic::Stable;
    
    //Define the base types
    using base_request_type = void;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_request_type = void;
    using mw_reply_type = ::Basic;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);

    auto requester_port = ConstructRequesterPort<zmq::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<zmq::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_port(*requester_port, 0));
    EXPECT_TRUE(setup_port(*replier_port, 0));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(zmq_ReqRep, LONG_Void2Basic_Busy){
    using namespace ::ReqRep::Void2Basic::Busy;
    
    //Define the base types
    using base_request_type = void;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_request_type = void;
    using mw_reply_type = ::Basic;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);

    auto requester_port = ConstructRequesterPort<zmq::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<zmq::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_port(*requester_port, 0));
    EXPECT_TRUE(setup_port(*replier_port, 0));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}