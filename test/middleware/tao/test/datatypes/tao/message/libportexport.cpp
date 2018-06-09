#include <core/ports/libportexport.h>

//Include the implementation and base message

#include "helloimpl.h"
#include "base/message/message.h"

#include <middleware/tao/requestreply/requesterport.hpp>
#include <middleware/tao/requestreply/replierport.hpp>



//std::string replier_orb_addr("iiop://192.168.111.90:50005");

bool setup_replier_port(Port& port, const std::string& orb_address, const std::string& publisher_name){
	auto pn = port.GetAttribute("publisher_name").lock();
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
	auto pa = port.GetAttribute("publisher_address").lock();
	auto pn = port.GetAttribute("publisher_name").lock();
	if(pa && pn && oa){
		oa->set_String(orb_address);
		pn->set_String(publisher_name);
		pa->set_String(publisher_address);
		return true;
	}
	return false;
};


int main(int argc, char** argv){
	const auto test_name = std::string("TAO");
	const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;


	auto component = std::make_shared<Component>(test_name);

    component->AddCallback<void, Base::Message>(rep_name, [](Base::Message& m) {
		std::cout << "Got Message: \n\t" << m.instName() << "\n\t" << m.get_messageID() << std::endl;
		});

	auto requester_port = ConstructRequesterPort<tao::RequesterPort<void, void, Base::Message, Test::Message, Test::Hello>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<void, void, Base::Message, Test::Message, ::Hello>>(rep_name, component);

	std::string replier_addr("192.168.1.108:50005");
	std::string requester_addr("192.168.1.108:50005");

	std::string replier_orb_addr("iiop://" + replier_addr);
	std::string requester_orb_addr("iiop://" + requester_addr);
	std::string replier_connect_addr("corbaloc:iiop:" + replier_addr);
	
	setup_requester_port(*requester_port, requester_orb_addr, "S1", replier_connect_addr + "/S1");
	setup_replier_port(*replier_port, replier_orb_addr, "S1");
	

	std::cerr << "Configured" << std::endl;

	requester_port->Configure();
	requester_port->Activate();
	std::cerr << "Activated" << std::endl;

	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	int i = 100;

	Base::Message message;
	message.messageID() = 0;

	while(i-- > 0){
		message.messageID()++;
		message.set_instName("Outport 1");
		requester_port->SendRequest(message, std::chrono::milliseconds(200));
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		if(i == 95){
			std::cerr << "LATE CONFIGURING THE REPLIER PORT" << std::endl;
			replier_port->Configure();
			replier_port->Activate();
		}
	}

	requester_port->Passivate();
	replier_port->Passivate();

	requester_port->Terminate();
	replier_port->Terminate();


	delete requester_port;
	delete replier_port;
	return 0;
};