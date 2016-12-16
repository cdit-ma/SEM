#include "receiverimpl.h"
#include <iostream>

void ReceiverImpl::rxMessage(Message* message){
    std::cout << instName() << " Got Message: "  << message->content() << " From: " << message->instName() << std::endl; 
}


void ReceiverImpl::rxVectorMessage(VectorMessage* message){
    int count = 0;
    for(auto e: message->data()){
         std::cout << "Element: " << count ++ << " Value: " << e << std::endl;
    }
}
