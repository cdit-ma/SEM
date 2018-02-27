#include <core/libportexport.h>

// Include the convert function
#include "convert.h"

#include "messageC.h"
#include "messageS.h"

#include <middleware/tao/outeventport.hpp>
#include <middleware/tao/ineventport.hpp>


EventPort* ConstructInEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	//return ConstructInEventPort<rti::InEventPort<Base::Message, Message> >(port_name, component);
	//return new tao::OutEventPort<Base::Message, Test::Message, Test::Hello>(component, port_name);
	return 0;
};

EventPort* ConstructOutEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	//return ConstructOutEventPort<rti::OutEventPort<Base::Message, Message> >(port_name, component);
	return 0;
};


std::string orb_addr("iiop://192.168.111.90:50003");

bool setup_in_port(EventPort& port, std::string publisher_name){
	auto pn = port.GetAttribute("publisher_name").lock();
	auto oa = port.GetAttribute("orb_endpoint").lock();
	if(pn && oa){
		pn->set_String(publisher_name);
		oa->set_String(orb_addr);
		return true;
	}
	return false;
}

bool setup_out_port(EventPort& port, std::vector<std::string> publisher_names, std::vector<std::string> publisher_addresses){
	auto pa = port.GetAttribute("publisher_address").lock();
	auto pn = port.GetAttribute("publisher_names").lock();
	auto oa = port.GetAttribute("orb_endpoint").lock();
	if(pa && pn && oa){
		oa->set_String(orb_addr);
		for(const auto& publisher_name : publisher_names){
			pn->StringList().push_back(publisher_name);
		}
		for(const auto& publisher_address : publisher_addresses){
			pa->StringList().push_back(publisher_address);
		}
		return true;
	}
	return false;
}


int main(int argc, char** argv){
	auto fn2 = [](Base::Message& m) {std::cout << "In Port 1: " << m.instName() << std::endl;};
	auto fn3 = [](Base::Message& m) {std::cout << "In Port 2: " << m.instName() << std::endl;};

	auto c = std::make_shared<Component>("Test");

	auto outport = new tao::OutEventPort<Base::Message, Test::Message, Test::Hello>(c, "TEST1");
	auto outport2 = new tao::OutEventPort<Base::Message, Test::Message, Test::Hello>(c, "TEST4");
	auto inport = new tao::InEventPort<Base::Message, Test::Message, POA_Test::Hello>(c, "TEST2", fn2);
	auto inport2 = new tao::InEventPort<Base::Message, Test::Message, POA_Test::Hello>(c, "TEST3", fn3);

	outport->set_id("1");
	outport2->set_id("2");
	inport->set_id("3");
	inport2->set_id("4");

	std::string server_addr("corbaloc:iiop:192.168.111.90:50003");

	{
		std::vector<std::string> publisher_names = {"S1", "Sender1"};
		std::vector<std::string> publisher_address = {server_addr + "/S1", "corbaloc:iiop:192.168.111.90:50005/Sender1"};
		setup_out_port(*outport, publisher_names, publisher_address);
	}
	{
		std::vector<std::string> publisher_names = {"S1", "S2", "Sender1"};
		std::vector<std::string> publisher_address = {server_addr + "/S1", server_addr + "/S2", "corbaloc:iiop:192.168.111.90:50005/Sender1"};
		setup_out_port(*outport2, publisher_names, publisher_address);
	}

	setup_in_port(*inport, "S1");
	setup_in_port(*inport2, "S2");

	inport->Configure();
	inport2->Configure();
	outport->Configure();
	outport2->Configure();
	std::cout << "Configured" << std::endl;

	inport->Activate();
	inport2->Activate();
	outport->Activate();
	outport2->Activate();
	std::cout << "Activated" << std::endl;

	

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	int i = 100;

	Base::Message message;
	message.messageID() = 0;

	while(i-- > 0){
		
		{
			message.messageID()++;
			if(i % 2){
				std::cout << ">>>>| Sent From: Outport 2 |<<<<" << std::endl;
				message.set_instName("Outport 2");
				outport2->tx(message);
			}else{
				std::cout << ">>>>| Sent From: Outport 1 |<<<<" << std::endl;
				message.set_instName("Outport 1");
				outport->tx(message);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	inport->Passivate();
	inport2->Passivate();

	outport->Passivate();
	outport2->Passivate();
	std::cout << "Passivated" << std::endl;

	inport->Terminate();
	std::cout << "Terminated" << std::endl;
	inport2->Terminate();
	std::cout << "Terminated" << std::endl;
	outport->Terminate();
	std::cout << "Terminated" << std::endl;
	outport2->Terminate();
	std::cout << "Terminated" << std::endl;

	delete inport;
	delete inport2;
	delete outport;
	delete outport2;

	return 0;
};