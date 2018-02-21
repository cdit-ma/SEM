#include <iostream>
#include "messageS.h"
#include <future>

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
    //std::string test("corbaloc:iiop:192.168.111.90:50001/Sender2");
    //auto object = orb->string_to_object(test.c_str());
//    orb->register_initial_reference("Sender2", object);

    std::string reference_str = "Sender";
    auto ref_obj = orb->resolve_initial_references(reference_str.c_str());

    if(!ref_obj){
        std::cerr << "Failed to resolve Reference '" << reference_str << "'" << std::endl;
    }

    Test::Message message;
    message.inst_name = "=D";
    message.time = argc;

    //auto sender = Test::Hello::_narrow(ref_obj);
    while(true){
        message.time++;
        std::cout << message.time << std::endl;
        auto sender = Test::Hello::_narrow(ref_obj);
        sender->send(message);
        CORBA::release(sender);
    }
    //CORBA::release(sender);

    orb->destroy();
    return 0;
}