#include <iostream>
#include "messageS.h"

int main(int argc, char** argv){
    auto orb = CORBA::ORB_init (argc, argv);
    ::CORBA::Object_var obj = orb->resolve_initial_references ("RootPOA");
    ::PortableServer::POA_var root_poa = ::PortableServer::POA::_narrow (obj.in ());

    // Activate the RootPOA's manager.
    ::PortableServer::POAManager_var mgr = root_poa->the_POAManager ();
    mgr->activate ();


    Test::Hello *sender = 0;
    ACE_NEW_RETURN (sender, Test::Hello (root_poa.in ()), 1);
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