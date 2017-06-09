#include <iostream>
#include "messageS.h"

int main(int argc, char** argv){

    //Get a pointer to the orb
    auto orb = CORBA::ORB_init (argc, argv);

    //Get the reference to the RootPOA
    ::CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    ::PortableServer::POA_var root_poa = ::PortableServer::POA::_narrow(obj.in());

    // Activate the RootPOA's manager.
    ::PortableServer::POAManager_var mgr = root_poa->the_POAManager();
    mgr->activate ();

    std::string reference_str = "LoggingServer";
    ::CORBA::Object_var ref_obj = orb->resolve_initial_references (reference_str.c_str());

    if(::CORBA::is_nil(ref_obj.in())){
        std::cerr << "Failed to resolve Reference '" << reference_str << "'" << std::endl;
    }

    //Narrow the 
    auto sender = Test::Hello::_narrow(ref_obj.in());
    Test::Hello_var sender_var = sender;

    if(CORBA::is_nil(sender_var.in())){
        std::cerr << "NILL REFERENCE Y'AL" << std::endl;
        return 0;
    }
    if(CORBA::is_nil(sender)){
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