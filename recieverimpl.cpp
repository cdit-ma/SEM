#include "recieverimpl.h"
#include <iostream>


void RecieverImpl::rxMessage(Message* message){
    std::cout << "RecieverImpl InstName: " << message->instName() << " Content: " << message->content() << " Time: " << message->time() << std::endl;
}
