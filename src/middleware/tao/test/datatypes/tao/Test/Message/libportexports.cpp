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


int main(int argc, char** argv){
	std::cout << "TESTING!" << std::endl;

	auto fn = [](Base::Aggregate* m) {std::cout << "LOL " << std::endl;};

	auto p = new tao::OutEventPort<Base::Aggregate, Test::Message, Test::Hello>(0, "TEST");
	
	auto p2 = new tao::InEventPort<Base::Aggregate, Test::Message, POA_Test::Hello>(0, "TEST2", fn);

	p->Activate();
	
	p2->Activate();
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	while(true){
		auto message = new Base::Aggregate();
		message->set_Member("HELLO FROM CUTS");
		p->tx(message);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
};