#include "helloreceiverimpl.h"
#include <iostream>

HelloReceiverImpl::HelloReceiverImpl(std::string name) : HelloReceiverInt(name){};

void HelloReceiverImpl::rxMessage(Message* message){
    std::cout << get_name() << " Got Message: "  << message->content() << " From: " << message->instName() << std::endl; 
}

/*void HelloReceiverImpl::rxVectorMessage(VectorMessage* message){
    int count = 0;
    for(auto e: message->data()){
         std::cout << "Element: " << count ++ << " Value: " << e << std::endl;
    }
}*/
