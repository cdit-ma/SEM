#include <iostream>
#include "messageS.h"

int main(int argc, char** argv){
    auto orb = CORBA::ORB_init (argc, argv);

    ::CORBA::Object_var obj = orb->resolve_initial_references ("RootPOA");
    ::PortableServer::POA_var root_poa = ::PortableServer::POA::_narrow (obj.in ());

    // Activate the RootPOA's manager.
    ::PortableServer::POAManager_var mgr = root_poa->the_POAManager ();
    mgr->activate ();


    std::cout << "Resolving LoggingServer" << std::endl;
    ::CORBA::Object_var obj = orb->resolve_initial_references ("LoggingServer");
    std::cout << "Resolved LoggingServer" << std::endl;
  if (::CORBA::is_nil (obj.in ()))
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("%T (%t) - %M - failed to resolve LoggingServer\n")),
                       -1);

    std::cout << "Connecting to server" << std::endl;
    auto sender = Test::Hello::_narrow(obj.in());

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