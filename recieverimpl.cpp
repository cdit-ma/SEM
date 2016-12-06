#include "recieverimpl.h"
#include <iostream>


void RecieverImpl::rxMessage(Message* message){
    std::cout << instName() << "RX Message: "  << message->content() << " From: " << message->instName() << std::endl; 
}
