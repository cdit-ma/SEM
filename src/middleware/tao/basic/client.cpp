#include <iostream>
#include "messageS.h"

int main(int argc, char** argv){

    ::CORBA::Object_var obj = this->orb_->resolve_initial_references ("RootPOA");
    ::PortableServer::POA_var root_poa = ::PortableServer::POA::_narrow (obj.in ());

    // Activate the RootPOA's manager.
    ::PortableServer::POAManager_var mgr = root_poa->the_POAManager ();
    mgr->activate ();

    auto sender = Test::Hello::_narrow(root_poa.in());

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