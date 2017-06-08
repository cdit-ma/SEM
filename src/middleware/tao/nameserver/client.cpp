#include <iostream>
#include <string> 
#include <sstream> 
#include "messageS.h"


std::string get_ior(std::string ip, int port, std::string poa_name, std::string obj_name){
    //corbaloc:iiop:192.168.111.90:12345/%14%01%0f%00NUP%00%00%00%1b%00%00%00%00%01%00%00%00RootPOA%00WHATISTHIS%00%00%00%00%00%01%00%00%00HelloSender
    std::stringstream ior;
    ior << "corbaloc:iiop:";
    ior << ip << ":" << port;
    ior << "/%14%01%0f%00NUP%00%00%00";
    int size = 17 + poa_name.size();
    //Append magic size
    ior << "%" << std::hex << size;
    ior << "%00%00%00%00%01%00%00%00RootPOA%00";
    ior << poa_name;
    ior << "%00%00%00%00%00%01%00%00%00";
    ior << obj_name;
    return ior.str();
}

int main(int argc, char** argv){
    auto orb = CORBA::ORB_init (argc, argv);
    //CORBA::Object_var ior = orb->string_to_object("corbaloc:iiop:1.2@192.168.111.90:12345/%14%01%0f%00NUP%00%00%00%19%00%00%00%00%01%00%00%00RootPOA%00childPOA%00%00%00%00%00%01%00%00%00Stock_Factory");

    auto ior_s = get_ior("192.168.111.90", 12345, "Test", "HelloSender");
    std::cout << ior_s << std::endl;
    CORBA::Object_var ior = orb->string_to_object(ior_s.c_str());
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