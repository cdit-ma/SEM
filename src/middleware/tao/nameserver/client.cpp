#include <iostream>
#include "messageS.h"

int main(int argc, char** argv){
    // Declare ORB
	CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

    // Get a reference to the Naming Service
    CORBA::Object_var rootContextObj = orb->resolve_initial_references("NameService");
    CosNaming::NamingContext_var nc = CosNaming::NamingContext::_narrow(rootContextObj.in());

    CosNaming::Name name;
    name.length(1);
    name[0].id = (const char *) "CryptographicService";
    name[0].kind = (const char *) "";
    // Invoke the root context to retrieve the object reference
    CORBA::Object_var managerObj = nc->resolve(name);
    // Narrow the previous object to obtain the correct type
    auto sender = Test::Hello::_narrow(managerObj.in());
    Test::Hello_var sender_var = sender;

    if(CORBA::is_nil (sender_var.in())){
        std::cerr << "NILL REFERENCE Y'AL" << std::endl;
        return 0;
    }

    auto message = Test::Message();
    message.inst_name = "test";
    message.time = argc;

    while(true){
        sender->send(message);
    }

    orb->destroy();
    return 0;
}