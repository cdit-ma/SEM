#include <core/libportexports.h>

#include "convert.h"
#include "messageC.h"
#include "messageS.h"

#include <middleware/tao/outeventport.hpp>
#include <middleware/tao/ineventport.hpp>

EventPort* ConstructRx(std::string port_name, Component* component){
	EventPort* p = 0;
	if(component){
		auto fn = component->GetCallback(port_name);
		if(fn){
			
			//p = new tao::InEventPort<Base::Aggregate, Test::Message, Test::Hello>(component, port_name, fn);
		}
	}
	return p;
};

EventPort* ConstructTx(std::string port_name, Component* component){
	return new tao::OutEventPort<Base::Aggregate, Test::Message, Test::Hello>(component, port_name);
};

std::map<std::string, ::Attribute*> get_in_attributes(std::string publisher_name, std::string publisher_address){
	std::map<std::string, ::Attribute*> attributes;
	attributes["publisher_address"] = new ::Attribute(ATTRIBUTE_TYPE::STRING, "publisher_address");
	attributes["publisher_address"]->set_String(publisher_address);
	attributes["publisher_name"] = new ::Attribute(ATTRIBUTE_TYPE::STRING, "publisher_name");
	attributes["publisher_name"]->set_String(publisher_name);
	return attributes;
};

std::map<std::string, ::Attribute*> get_out_attributes(std::vector<std::string> publisher_address, std::vector<std::string> publisher_references){
	std::map<std::string, ::Attribute*> attributes;
	attributes["publisher_references"] = new ::Attribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_references");
	attributes["publisher_references"]->set_StringList(publisher_references);
	attributes["publisher_address"] = new ::Attribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address");
	attributes["publisher_address"]->set_StringList(publisher_address);
	return attributes;
};


int main(int argc, char** argv){
	auto fn2 = [](Base::Aggregate* m) {std::cout << "tao::InEventPort<2>: " << m->Member() << std::endl;};
	auto fn3 = [](Base::Aggregate* m) {std::cout << "tao::InEventPort<3>: " << m->Member() << std::endl;};

	auto p = new tao::OutEventPort<Base::Aggregate, Test::Message, Test::Hello>(0, "TEST");
	auto p2 = new tao::InEventPort<Base::Aggregate, Test::Message, POA_Test::Hello>(0, "TEST2", fn2);
	auto p3 = new tao::InEventPort<Base::Aggregate, Test::Message, POA_Test::Hello>(0, "TEST3", fn3);
	auto p4 = new tao::OutEventPort<Base::Aggregate, Test::Message, Test::Hello>(0, "TEST4");

	{
		std::vector<std::string> references = {"TestServer", "TestServer2", "LoggingServer2"};
		std::vector<std::string> publisher_address = {"TestServer=corbaloc:iiop:192.168.111.90:50002/TestServer", "TestServer2=corbaloc:iiop:192.168.111.90:50004/TestServer2", "LoggingServer2=corbaloc:iiop:192.168.111.90:50003/LoggingServer2"};
		p->Startup(get_out_attributes(publisher_address, references));
	}
	{
		std::vector<std::string> references = {"TestServer2", "LoggingServer2"};
		std::vector<std::string> publisher_address = {"TestServer=corbaloc:iiop:192.168.111.90:50002/TestServer", "TestServer2=corbaloc:iiop:192.168.111.90:50004/TestServer2", "LoggingServer2=corbaloc:iiop:192.168.111.90:50003/LoggingServer2"};
		p4->Startup(get_out_attributes(publisher_address, references));
	}
	p2->Startup(get_in_attributes("TestServer", "iiop://192.168.111.90:50002"));
	p3->Startup(get_in_attributes("TestServer2", "iiop://192.168.111.90:50004"));
	
	p->Activate();
	p2->Activate();
	p3->Activate();
	p4->Activate();

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	int i = 10;
	while(i-- > 0){
		
		{
			auto message = new Base::Aggregate();
			message->set_Member("tao::OutEventPort<1>");
			p->tx(message);
		}
		{
			auto message = new Base::Aggregate();
			message->set_Member("tao::OutEventPort<4>");
			p4->tx(message);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << std::endl;
	}


	p2->Passivate();
	p3->Passivate();
	p->Passivate();
	p4->Passivate();

	return 0;
};