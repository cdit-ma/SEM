#include "messageS.h"
#include "../../../base/basic.hpp"
#include <core/ports/libportexport.h>
#define TAO_SERVER_FUNC_NAME send
#include <middleware/tao/requestreply/replierport.hpp>
#include <middleware/tao/requestreply/requesterport.hpp>
#include <re_common/util/execution.hpp>


Execution execution;

void signal_handler (int signal_value){
    execution.Interrupt();
}


const std::string ns_addr("192.168.111.96");
const std::string ns_port("35701");



const std::string ip_addr("192.168.111.96");
const std::string replier_addr(ip_addr + ":5003");
const std::string requester_addr(ip_addr + ":5010");

const std::string replier_orb_addr("iiop://" + replier_addr);
const std::string requester_orb_addr("iiop://" + requester_addr);

const std::string replier_connect_addr("corbaloc:iiop:" + replier_addr);
const std::string ns_connect_address("corbaloc:iiop:" + ns_addr + ":" + ns_port);


bool setup_port(Port& port, const std::string& orb_address, const std::string& name_server_endpoint, const std::vector<std::string>& server_name){
    auto orb_attr = port.GetAttribute("orb_endpoint").lock();
	auto ns_attr = port.GetAttribute("naming_service_endpoint").lock();
	auto sn_attr = port.GetAttribute("server_name").lock();
	if(orb_attr && ns_attr && sn_attr){
		orb_attr->set_String(orb_address);
		ns_attr->set_String(name_server_endpoint);
		sn_attr->set_StringList(server_name);
		return true;
	}
	return false;
};

Base::Basic Callback(Base::Basic& message){
    std::cerr << "GOT: " << message.int_val << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    message.int_val *= 10;
    return message;
};

int main(int, char**){
    //Handle the SIGINT/SIGTERM signal
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    //Define the base types
    using base_request_type = Base::Basic;
    using base_reply_type = Base::Basic;
    
    //Define the proto types
    using mw_reply_type = ::Basic;
    using mw_request_type = ::Basic;

    using mw_reply_server_type = ::POA_Basic2Basic;
    using mw_reply_client_type = ::Basic2Basic;

    const auto test_name = std::string("Hello");
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_server_type>>(rep_name, component);
    
    setup_port(*replier_port, replier_orb_addr, ns_connect_address, {"TEST", "POOPYFACETOMATONOSE", rep_name});

    auto& helper = tao::TaoHelper::get_tao_helper();
    auto orb = helper.get_orb(replier_orb_addr);
    helper.register_initial_reference(orb, "NamingService", ns_connect_address);
    std::cerr << "Registered Naming Service as: " << ns_connect_address  << std::endl;


    std::cerr << (replier_port->Configure() ? "Configured" : "FAILED") << std::endl;
    std::cerr << (replier_port->Activate() ? "Activated" : "FAILED") << std::endl;
    execution.Start();
    std::cerr << (replier_port->Passivate() ? "Passivated" : "FAILED") << std::endl;
    std::cerr << (replier_port->Terminate() ? "Terminated" : "FAILED") << std::endl;
    delete replier_port;
    return 0;
}


