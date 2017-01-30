#include "helloreceiverimpl.h"
#include <iostream>

HelloReceiverImpl::HelloReceiverImpl(std::string name) : HelloReceiverInt(name){};

void HelloReceiverImpl::rxMessage(Message* message){
    std::cout << get_name() << " Got Message: "  << message->content() << " From: " << message->instName() << std::endl; 
}

Component* HelloReceiverImpl::constructor_fn(std::string name){
    return new HelloReceiverImpl(name);
}
void HelloReceiverImpl::destructor_fn(Component* c){
    auto c2 = static_cast<HelloReceiverImpl*>(c);
    if(c2){
        delete c2;
    }
}


/*void HelloReceiverImpl::rxVectorMessage(VectorMessage* message){
    int count = 0;
    for(auto e: message->data()){
         std::cout << "Element: " << count ++ << " Value: " << e << std::endl;
    }
}*/
