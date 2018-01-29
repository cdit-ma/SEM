#include <iostream>
#include "messageS.h"

int main(int argc, char** argv){

    std::cout << "ORB BEFORE OPTIONS: ";
    for(int i = 0; i < argc; i++){
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;

    //Get a pointer to the orb
    auto orb = CORBA::ORB_init (argc, argv);

    std::cout << "ORB AFTER OPTIONS: ";
    for(int i = 0; i < argc; i++){
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;


    //Get the reference to the RootPOA
    //auto obj = orb->resolve_initial_references("RootPOA");
    //auto root_poa = ::PortableServer::POA::_narrow(obj);

    std::string reference_str = "LoggingServer";
    auto ref_obj = orb->resolve_initial_references(reference_str.c_str());

    if(!ref_obj){
        std::cerr << "Failed to resolve Reference '" << reference_str << "'" << std::endl;
    }
    std::cout << "GOT REFERENCE MATE" << std::endl;

    //Convert the ref_obj into a typed writer
    auto sender = Test::Hello::_narrow(ref_obj);

    if(!sender){
        std::cerr << "NILL REFERENCE Y'AL" << std::endl;
    }
    std::cout << "GOT REFERENCE MATE" << std::endl;

    Test::Message message;
    message.inst_name = "=D";
    message.time = argc;

    while(true){
        sender->send(message);
        message.time++;
    }

    orb->destroy();
    return 0;
}