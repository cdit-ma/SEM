#include <core/libportexport.h>

// Include the convert function
#include "convert.h"

#include "messageC.h"
#include "messageS.h"

#include <middleware/tao/outeventport.hpp>

#include <map>
//include <middleware/tao/ineventport.hpp>

EventPort* ConstructInEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	//return ConstructInEventPort<rti::InEventPort<Base::Message, Message> >(port_name, component);
	//return new tao::OutEventPort<Base::Message, Test::Message, Test::Hello>(component, port_name);
	return 0;
};

EventPort* ConstructOutEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	//return ConstructOutEventPort<rti::OutEventPort<Base::Message, Message> >(port_name, component);
	return 0;
};

bool setup_in_port(EventPort& port, std::string publisher_name, std::string publisher_address){
	auto pa = port.GetAttribute("publisher_address").lock();
	auto pn = port.GetAttribute("publisher_name").lock();
	if(pa && pn){
		pa->set_String(publisher_address);
		pn->set_String(publisher_name);
		return true;
	}
	return false;
}

bool setup_out_port(EventPort& port, std::vector<std::string> publisher_names, std::vector<std::string> publisher_addresses){
	auto pa = port.GetAttribute("publisher_address").lock();
	auto pn = port.GetAttribute("publisher_name").lock();
	if(pa && pn){
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
	auto fn2 = [](Base::Message& m) {std::cout << "tao::InEventPort<2>: " << m.instName() << std::endl;};
	auto fn3 = [](Base::Message& m) {std::cout << "tao::InEventPort<3>: " << m.instName() << std::endl;};

	auto c = std::make_shared<Component>("Test");

	auto p = new tao::OutEventPort<Base::Message, Test::Message, Test::Hello>(c, "TEST");
	//auto p2 = new tao::InEventPort<Base::Message, Test::Message, POA_Test::Hello>(c, "TEST2", fn2);
	//auto p3 = new tao::InEventPort<Base::Message, Test::Message, POA_Test::Hello>(c, "TEST3", fn3);
	auto p4 = new tao::OutEventPort<Base::Message, Test::Message, Test::Hello>(c, "TEST4");

	{
		std::vector<std::string> publisher_names = {"TestServer", "TestServer2", "LoggingServer2"};
		std::vector<std::string> publisher_address = {"TestServer=corbaloc:iiop:192.168.111.90:50002/TestServer", "TestServer2=corbaloc:iiop:192.168.111.90:50004/TestServer2", "LoggingServer2=corbaloc:iiop:192.168.111.90:50003/LoggingServer2"};
		setup_out_port(*p, publisher_names, publisher_address);
	}
	{
		std::vector<std::string> publisher_names = {"TestServer2", "LoggingServer2"};
		std::vector<std::string> publisher_address = {"TestServer=corbaloc:iiop:192.168.111.90:50002/TestServer", "TestServer2=corbaloc:iiop:192.168.111.90:50004/TestServer2", "LoggingServer2=corbaloc:iiop:192.168.111.90:50003/LoggingServer2"};
		setup_out_port(*p, publisher_names, publisher_address);
	}
	//p2->Startup(get_in_attributes("TestServer", "iiop://192.168.111.90:50002"));
	//p3->Startup(get_in_attributes("TestServer2", "iiop://192.168.111.90:50004"));
	
	p->Activate();
	//p2->Activate();
	//p3->Activate();
	p4->Activate();

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	int i = 10;
	while(i-- > 0){
		
		{
			auto message = new Base::Message();
			message->set_instName("tao::OutEventPort<1>");
			p->tx(*message);
		}
		{
			auto message = new Base::Message();
			message->set_instName("tao::OutEventPort<4>");
			p4->tx(*message);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << std::endl;
	}


	//p2->Passivate();
	//p3->Passivate();
	p->Passivate();
	p4->Passivate();

	return 0;
};