#include "proxyimpl.h"
#include <iostream>

ProxyImpl::ProxyImpl(std::string name) : ProxyInt(name){};

void ProxyImpl::rxMessage(::Message* message){
    std::cout << get_name() << " Got Message: "  << message->content() << " From: " << message->instName() << std::endl; 
    txMessage(message);
}
