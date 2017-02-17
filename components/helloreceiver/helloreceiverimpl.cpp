#include "helloreceiverimpl.h"
#include <iostream>

HelloReceiverImpl::HelloReceiverImpl(std::string name) : HelloReceiverInt(name){};

void HelloReceiverImpl::rxMessage(Message* message){
    std::cout << get_name() << " Got Message: "  << message->content() << " From: " << message->instName() << std::endl; 
}

void HelloReceiverImpl::rxVectorMessage(VectorMessage message){
    int count = 0;
    for(auto e: message.data()){
         std::cout << "Element: " << count ++ << " Value: " << e << std::endl;
    }

     for(auto e: message.Vector_Inst()){
         std::cout << "GOT COMPLEX TYPE: " << e.time() << std::endl;
    }

    std::cout << get_name() << " Got rxVectorMessage->message: "  << message.msgInst().content() << " From: " << message.msgInst().instName() << std::endl; 
}
