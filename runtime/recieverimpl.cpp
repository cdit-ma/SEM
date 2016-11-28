#include "recieverimpl.h"
#include <iostream>


void RecieverImpl::rxMessage(Message* message){
    std::cout << "RecieverImpl GOT: " << message->get_base_message_id() << std::endl;
}
