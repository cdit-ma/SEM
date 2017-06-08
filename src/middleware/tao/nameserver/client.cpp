#include <iostream>
#include "messageS.h"

int main(int argc, char** argv){
    auto orb = CORBA::ORB_init (argc, argv);
    CORBA::Object_var ior = orb->string_to_object("corbaloc:iiop:1.2@192.168.111.90:12345/%14%01%0f%00NUP%00%00%00%19%00%00%00%00%01%00%00%00RootPOA%00childPOA%00%00%00%00%00%01%00%00%00Stock_Factory");
    
    auto sender = Test::Hello::_narrow(ior.in());

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