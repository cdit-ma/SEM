
//Include the implementation and base message

//#include "helloimpl.h"
#include "base/message/message.h"
#include "messageS.h"


#include <core/ports/libportexport.h>

#define TAO_SERVER_FUNC_NAME send
#include <middleware/tao/requestreply/requesterport.hpp>
#include <middleware/tao/requestreply/replierport.hpp>
#undef TAO_SERVER_FUNC_NAME



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
		std::cerr << "server_name: " << publisher_name << std::endl;
		std::cerr << "server_address: " << publisher_address << std::endl;
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
	const auto rep2_name = "rp_" + test_name;
	const auto rep3_name = "rp3_" + test_name;
	const auto rep4_name = "rp4_" + test_name;

	auto component = std::make_shared<Component>(test_name);
	auto component2 = std::make_shared<Component>("2");

    component->AddCallback<void, Base::Message>(rep_name, [](Base::Message& m) {
		std::cout << "RP1: Got Message: \n\t" << m.instName() << "\n\t" << m.get_messageID() << std::endl;
		});


	component->AddCallback<Base::Message, Base::Message>(rep3_name, [](Base::Message& m) -> Base::Message {
		std::cout << "Twenty 3: Got Message: \n\t" << m.instName() << "\n\t" << m.get_messageID() << std::endl;
		m.set_instName("Testy");
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
		m.set_messageID(m.get_messageID() * 10);
		std::cout << "Sending Reply" << std::endl;
		return m;
	});

	component2->AddCallback<void, Base::Message>(rep_name, [](Base::Message& m) {
		std::cout << "RP2: Message: \n\t" << m.instName() << "\n\t" << m.get_messageID() << std::endl;
		});

	auto requester_port = ConstructRequesterPort<tao::RequesterPort<void, void, Base::Message, Test::Message, Test::Hello>>(req_name, component);
	auto requester2_port = ConstructRequesterPort<tao::RequesterPort<Base::Message, Test::Message, Base::Message, Test::Message, Test::Hello2>>(req_name, component);
    auto replier_port = ConstructReplierPort<tao::ReplierPort<void, void, Base::Message, Test::Message, POA_Test::Hello>>(rep_name, component);
	auto replier2_port = ConstructReplierPort<tao::ReplierPort<void, void, Base::Message, Test::Message, POA_Test::Hello>>(rep_name, component2);
	auto replier3_port = ConstructReplierPort<tao::ReplierPort<Base::Message, Test::Message, Base::Message, Test::Message, POA_Test::Hello2>>(rep3_name, component);

	std::string replier_addr("192.168.111.90:50005");
	std::string replier2_addr("192.168.111.90:50006");
	std::string requester_addr("192.168.111.90:50005");

	std::string replier_orb_addr("iiop://" + replier_addr);
	std::string replier2_orb_addr("iiop://" + replier2_addr);
	std::string requester_orb_addr("iiop://" + requester_addr);

	std::string replier_connect_addr("corbaloc:iiop:" + replier_addr);
	std::string replier2_connect_addr("corbaloc:iiop:" + replier2_addr);
	
	
	setup_replier_port(*replier_port, replier_orb_addr, rep_name);
	setup_replier_port(*replier2_port, replier2_orb_addr, rep_name);
	setup_replier_port(*replier3_port, replier2_orb_addr, rep3_name);
	
	replier_port->Configure();
	replier_port->Activate();

	replier2_port->Configure();
	replier2_port->Activate();
	
	replier3_port->Configure();
	replier3_port->Activate();

	int i = 10;

	Base::Message message;
	message.messageID() = 0;
	while(i-- > 0){

		message.set_instName("Requester 3");
		setup_requester_port(*requester2_port, requester_orb_addr, rep_name, replier2_connect_addr + "/" + rep3_name);
		
		requester2_port->Configure();
		requester2_port->Activate();

		message.messageID()++;
		
		std::cerr << "SendingRequest" << std::endl;
		auto result = requester2_port->SendRequest(message, std::chrono::milliseconds(1));
		if(result.first){
			std::cerr << "GOT Reply" << std::endl;
			std::cerr << "RESULT: " << result.second.instName() << std::endl;
			std::cerr << "RESULT: " << result.second.get_messageID() << std::endl;
		}else{
			std::cerr << "\t\t\t\t\t\t\t\t DED" << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		requester2_port->Terminate();

		if( i == 5){
			delete replier3_port;
		}
		if(i == 4){
			replier3_port = ConstructReplierPort<tao::ReplierPort<Base::Message, Test::Message, Base::Message, Test::Message, POA_Test::Hello2>>(rep3_name, component);
			setup_replier_port(*replier3_port, replier2_orb_addr, rep3_name);
			replier3_port->Configure();
			replier3_port->Activate();
		}
	}
	





	/*std::cerr << "Passivate" << std::endl;
	requester_port->Passivate();
	requester2_port->Passivate();
	replier_port->Passivate();
	replier2_port->Passivate();
	replier3_port->Passivate();
	std::cerr << "Passivated" << std::endl;

	std::cerr << "Terminate" << std::endl;
	requester_port->Terminate();
	requester2_port->Terminate();
	replier_port->Terminate();
	replier2_port->Terminate();
	replier3_port->Terminate();
	std::cerr << "Terminated" << std::endl;*/


	delete requester_port;
	delete requester2_port;
	delete replier_port;
	delete replier2_port;
	delete replier3_port;

	return 0;
};