#include "receiverimpl.h"
#include <iostream>


void ReceiverImpl::rxMessage(Message* message){
    std::cout << instName() << "RX Message: "  << message->content() << " From: " << message->instName() << std::endl; 
}
