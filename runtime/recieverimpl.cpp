#include "recieverimpl.h"
#include <iostream>


void RecieverImpl::rxMessage(Message* message){
    std::cout << "Recieved Message: "  << message->content() << " From: " << message->instName() << std::endl; 
}
