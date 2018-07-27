#include "messageS.h"
#include "../../../base/basic.hpp"
#include <core/ports/libportexport.h>
#define TAO_SERVER_FUNC_NAME send
#include <middleware/tao/requestreply/replierport.hpp>
#include <middleware/tao/requestreply/requesterport.hpp>

const std::string ns_addr("192.168.111.96");
const std::string ns_port("33283");



const std::string ip_addr("192.168.111.96");
const std::string replier_addr(ip_addr + ":5002");
const std::string requester_addr(ip_addr + ":5010");

const std::string replier_orb_addr("iiop://" + replier_addr);
const std::string requester_orb_addr("iiop://" + requester_addr);

const std::string replier_connect_addr("corbaloc:iiop:" + replier_addr);
const std::string ns_connect_address("corbaloc:iiop:" + ns_addr + ":" + ns_port);


bool setup_replier_port(Port& port, const std::string& orb_address, const std::string& publisher_name){
	auto pn = port.GetAttribute("server_name").lock();
	auto oa = port.GetAttribute("orb_endpoint").lock();
	if(pn && oa){
		oa->set_String(orb_address);
		pn->set_String(publisher_name);
		return true;
	}
	return false;
};

bool setup_requester_port(Port& port, const std::string& orb_address, const std::string& publisher_name, const std::string& publisher_address){
    auto oa = port.GetAttribute("orb_endpoint").lock();
	auto pa = port.GetAttribute("server_address").lock();
	auto pn = port.GetAttribute("server_name").lock();
	if(pa && pn && oa){
		//std::cerr << "server_name: " << publisher_name << std::endl;
		//std::cerr << "server_address: " << publisher_address << std::endl;
		oa->set_String(orb_address);
		pn->set_String(publisher_name);
		pa->set_String(publisher_address);
		return true;
	}
	return false;
}

Base::Basic Callback(Base::Basic& message){
    //std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    message.int_val *= 10;
    return message;
};

int main(int, char**){
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
    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);

    
    setup_requester_port(*requester_port, requester_orb_addr, rep_name, replier_connect_addr + "/" + rep_name);

    auto& helper = tao::TaoHelper::get_tao_helper();
    auto orb = helper.get_orb(requester_orb_addr);
    helper.register_initial_reference(orb, "NamingService", ns_connect_address);
    std::cerr << "Registered Naming Service as: " << ns_connect_address  << std::endl;


    std::cerr << (requester_port->Configure() ? "Configured" : "FAILED") << std::endl;
    std::cerr << (requester_port->Activate() ? "Activated" : "FAILED") << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    for(int i = 0 ; i < 1000; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        std::cerr << "TX: " << i << std::endl;
        auto c = requester_port->SendRequest(b, std::chrono::milliseconds(250));
        if(c.first){
            std::cerr << "GOT RX: " << i << std::endl;
            std::cerr << c.second.int_val << std::endl;
            std::cerr << c.second.str_val << std::endl;
        }else{
            i--;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cerr << (requester_port->Passivate() ? "Passivated" : "FAILED") << std::endl;
    std::cerr << (requester_port->Terminate() ? "Terminated" : "FAILED") << std::endl;
    delete requester_port;
    return 0;
}


