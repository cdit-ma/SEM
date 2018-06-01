#include <core/libportexport.h>

//Include the implementation and base message
#include "helloimpl.h"
#include "base/message/message.h"

#include <middleware/tao/outeventport.hpp>
#include <middleware/tao/ineventport.hpp>

EventPort* ConstructInEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	return ConstructInEventPort<tao::InEventPort<Base::Message, Test::Message, Test::HelloImpl> >(port_name, component);
};

EventPort* ConstructOutEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	return ConstructOutEventPort<tao::OutEventPort<Base::Message, Test::Message, Test::Hello> >(port_name, component);
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
	auto c = std::make_shared<Component>("Test");
	c->AddCallback<Base::Message>("InEventPort1", [](Base::Message& m) {std::cout << "In Port 1: " << m.instName() << std::endl;});
	c->AddCallback<Base::Message>("InEventPort2", [](Base::Message& m) {std::cout << "In Port 2: " << m.instName() << std::endl;});

	auto outport = new tao::OutEventPort<Base::Message, Test::Message, Test::Hello>(c, "TEST1");
	auto outport2 = new tao::OutEventPort<Base::Message, Test::Message, Test::Hello>(c, "TEST4");
	auto inport = new tao::InEventPort<Base::Message, Test::Message, Test::HelloImpl>(c, "InEventPort1", c->GetCallback("InEventPort1"));
	auto inport2 = new tao::InEventPort<Base::Message, Test::Message, Test::HelloImpl>(c, "InEventPort2", c->GetCallback("InEventPort2"));

	outport->set_id("1");
	outport2->set_id("2");
	inport->set_id("3");
	inport2->set_id("4");

	std::string server_addr("corbaloc:iiop:192.168.111.90:50003");

	{
		//std::vector<std::string> publisher_names = {"S1", "Sender1"};
		//std::vector<std::string> publisher_address = {server_addr + "/S1", "corbaloc:iiop:192.168.111.90:50005/Sender1"};
		//std::vector<std::string> publisher_names = {"Sender1"};
		//std::vector<std::string> publisher_address = {"corbaloc:iiop:192.168.111.90:50005/Sender1"};

		std::vector<std::string> publisher_names = {"S2"};
		std::vector<std::string> publisher_address = {server_addr + "/S2"};
		setup_out_port(*outport, publisher_names, publisher_address);
	}
	{
		// std::vector<std::string> publisher_names = {"S1", "S2", "Sender1"};
		// std::vector<std::string> publisher_address = {server_addr + "/S1", server_addr + "/S2", "corbaloc:iiop:192.168.111.90:50005/Sender1"};

		std::vector<std::string> publisher_names = {"S1"};
		std::vector<std::string> publisher_address = {server_addr + "/S1"};
		//std::vector<std::string> publisher_names = {"Sender1"};
		//std::vector<std::string> publisher_address = {"corbaloc:iiop:192.168.111.90:50005/Sender1"};
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
	//outport2->Activate();
	std::cout << "Activated" << std::endl;

	
	std::cout << "SLEEPY" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << "AWAKE" << std::endl;

	int i = 100;

	Base::Message message;
	message.messageID() = 0;

	while(i-- > 0){
		
		{
			message.messageID()++;
			/*if(i % 2){
				std::cout << ">>>>| Sent From: Outport 2 |<<<<" << std::endl;
				message.set_instName("Outport 2");
				outport2->tx(message);
				std::cout << ">>>>| Sent |<<<<" << std::endl;
			}else{*/
				std::cout << ">>>>| Sending From: Outport 1 |<<<<" << std::endl;
				message.set_instName("Outport 1");
				outport->tx(message);
				std::cout << ">>>>| Sent |<<<<" << std::endl;
			//}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100000));

	inport->Passivate();
	//inport2->Passivate();

	outport->Passivate();
	outport2->Passivate();
	std::cout << "Passivated" << std::endl;

	inport->Terminate();
	//inport2->Terminate();
	outport->Terminate();
	outport2->Terminate();

	delete inport;
	delete inport2;
	delete outport;
	delete outport2;

	return 0;
};