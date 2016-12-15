#include "receiverimpl.h"
#include <iostream>


void receiverImpl::rxMessage(Message* message){
    std::cout << instName() << "RX Message: "  << message->content() << " From: " << message->instName() << std::endl; 
}
