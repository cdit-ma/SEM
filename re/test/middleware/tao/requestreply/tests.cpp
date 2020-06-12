#include "gtest/gtest.h"
#include "../../../core/activatablefsmtester.h"

//Include the premade reqrep tests
#include "../../base/reqreptests.hpp"

#include "messageS.h"

#include <core/ports/libportexport.h>
#define TAO_SERVER_FUNC_NAME send
#include <middleware/tao/requestreply/replierport.hpp>
#include <middleware/tao/requestreply/requesterport.hpp>

//Include mains
#include "../../../core/test_main.h"

const std::string ip_addr("127.0.0.1");

const std::string replier_addr(ip_addr + ":5000");
const std::string requester_addr(ip_addr + ":5001");
const std::string replier_orb_addr("iiop://" + replier_addr);
const std::string requester_orb_addr("iiop://" + requester_addr);


bool setup_replier_port(Port& port, const std::string& orb_address, const std::string& publisher_name){
	auto oa = port.GetAttribute("orb_endpoint").lock();
	auto sn = port.GetAttribute("server_name").lock();
    auto sk = port.GetAttribute("server_kind").lock();
    auto nse = port.GetAttribute("naming_service_endpoint").lock();

	if(oa && sn && sk && nse){
		oa->set_String(orb_address);
		sn->set_StringList({cditma::GetNodeName(), publisher_name});
		sk->set_String("TEST_PORT");
        nse->set_String(cditma::GetTaoNamingServerAddress());
		return true;
	}
	return false;
};

bool setup_requester_port(Port& port, const std::string& orb_address, const std::string& publisher_name){
    auto oa = port.GetAttribute("orb_endpoint").lock();
	auto sn = port.GetAttribute("server_name").lock();
    auto sk = port.GetAttribute("server_kind").lock();
    auto nse = port.GetAttribute("naming_service_endpoint").lock();

	if(oa && sn && sk && nse){
		oa->set_String(orb_address);
		sn->set_StringList({cditma::GetNodeName(),publisher_name});
		sk->set_String("TEST_PORT");
        nse->set_String(cditma::GetTaoNamingServerAddress());

		return true;
	}
	return false;
}

Base::Basic EmptyCallback(Base::Basic& m){
    return m;
};



//Define an Requester Port FSM Tester
class Re_Port_Req_Tao_FSM : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = ConstructRequesterPort<tao::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic, ::Basic2Basic>>(port_name, component);
            EXPECT_TRUE(setup_requester_port(*port, requester_orb_addr, port_name));
            a = port;
            ASSERT_TRUE(a);
        }
};

//Define an Replier Port FSM Tester
class Re_Port_Rep_Tao_FSM : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            component->RegisterCallback<Base::Basic, Base::Basic>(port_name, EmptyCallback);
            auto port = ConstructReplierPort<tao::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic, POA_Basic2Basic>>(port_name, component);
            
            EXPECT_TRUE(setup_replier_port(*port, replier_orb_addr, port_name));
            a = port;
            ASSERT_TRUE(a);
        }
};

#define TEST_FSM_CLASS Re_Port_Req_Tao_FSM
//#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS Re_Port_Rep_Tao_FSM
//#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS



TEST(Re_Port_ReqRep_Tao, Basic2Basic_Stable){
    using namespace ::ReqRep::Basic2Basic::Stable;

    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_reply_type = ::Basic;
    using mw_request_type = ::Basic;

    using mw_reply_server_type = ::POA_Basic2Basic;
    using mw_reply_client_type = ::Basic2Basic;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);
    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_server_type>>(rep_name, component);
    

    

    EXPECT_TRUE(setup_requester_port(*requester_port, requester_orb_addr, rep_name));
    EXPECT_TRUE(setup_replier_port(*replier_port, replier_orb_addr, rep_name));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(Re_Port_ReqRep_Tao, Basic2Basic_Busy_LONG){
    using namespace ::ReqRep::Basic2Basic::Busy;

    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_reply_type = ::Basic;
    using mw_request_type = ::Basic;

    using mw_reply_server_type = ::POA_Basic2Basic;
    using mw_reply_client_type = ::Basic2Basic;


    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);
    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_server_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_requester_port(*requester_port, requester_orb_addr, rep_name));
    EXPECT_TRUE(setup_replier_port(*replier_port, replier_orb_addr, rep_name));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(Re_Port_ReqRep_Tao, Basic2Basic_Timeout_LONG){
    using namespace ::ReqRep::Basic2Basic::Timeout;
    
    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_reply_type = ::Basic;
    using mw_request_type = ::Basic;

    using mw_reply_server_type = ::POA_Basic2Basic;
    using mw_reply_client_type = ::Basic2Basic;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);
    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_server_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_requester_port(*requester_port, requester_orb_addr, rep_name));
    EXPECT_TRUE(setup_replier_port(*replier_port, replier_orb_addr, rep_name));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(Re_Port_ReqRep_Tao, Basic2Void_Stable){
    using namespace ::ReqRep::Basic2Void::Stable;
    
    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = void;
    
    //Define the proto types
    using mw_request_type = ::Basic;
    using mw_reply_type = void;

    using mw_reply_server_type = ::POA_Basic2Void;
    using mw_reply_client_type = ::Basic2Void;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);

    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_server_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_requester_port(*requester_port, requester_orb_addr, rep_name));
    EXPECT_TRUE(setup_replier_port(*replier_port, replier_orb_addr, rep_name));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(Re_Port_ReqRep_Tao, Basic2Void_Busy_LONG){
    using namespace ::ReqRep::Basic2Void::Busy;
    
    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = void;
    
    //Define the proto types
    using mw_request_type = ::Basic;
    using mw_reply_type = void;

    using mw_reply_server_type = ::POA_Basic2Void;
    using mw_reply_client_type = ::Basic2Void;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);

    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_server_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_requester_port(*requester_port, requester_orb_addr, rep_name));
    EXPECT_TRUE(setup_replier_port(*replier_port, replier_orb_addr, rep_name));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}


TEST(Re_Port_ReqRep_Tao, Basic2Void_Timeout_LONG){
    using namespace ::ReqRep::Basic2Void::Timeout;
    
    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = void;
    
    //Define the proto types
    using mw_request_type = ::Basic;
    using mw_reply_type = void;

    using mw_reply_server_type = ::POA_Basic2Void;
    using mw_reply_client_type = ::Basic2Void;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);

    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_server_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_requester_port(*requester_port, requester_orb_addr, rep_name));
    EXPECT_TRUE(setup_replier_port(*replier_port, replier_orb_addr, rep_name));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(Re_Port_ReqRep_Tao, Void2Basic_Stable){
    using namespace ::ReqRep::Void2Basic::Stable;
    
    //Define the base types
    using base_request_type = void;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_request_type = void;
    using mw_reply_type = ::Basic;

    using mw_reply_server_type = ::POA_Void2Basic;
    using mw_reply_client_type = ::Void2Basic;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);

    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_server_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_requester_port(*requester_port, requester_orb_addr, rep_name));
    EXPECT_TRUE(setup_replier_port(*replier_port, replier_orb_addr, rep_name));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

TEST(Re_Port_ReqRep_Tao, Void2Basic_Busy_LONG){
    using namespace ::ReqRep::Void2Basic::Busy;
    
    //Define the base types
    using base_request_type = void;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_request_type = void;
    using mw_reply_type = ::Basic;

    using mw_reply_server_type = ::POA_Void2Basic;
    using mw_reply_client_type = ::Void2Basic;

    const auto test_name = get_long_test_name();
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);

    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_server_type>>(rep_name, component);
    
    EXPECT_TRUE(setup_requester_port(*requester_port, requester_orb_addr, rep_name));
    EXPECT_TRUE(setup_replier_port(*replier_port, replier_orb_addr, rep_name));

    RunTest(*requester_port, *replier_port);

    delete requester_port;
    delete replier_port;
}

