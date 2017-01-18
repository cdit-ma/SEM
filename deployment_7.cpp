#include "deployment_7.h"

#include <iostream>

#include "common/includes/core/periodiceventport.h"

//Implementations for the components
#include "senderimpl.h"

//RTI Implementation of the event ports
#include "opensplice/ospltxmessage.h"

SenderImpl* construct_sender_impl(NodeContainer* c, std::string name){
    SenderImpl* s = new SenderImpl(name);
    if(c->add_component(s)){
        //Do specifics
        PeriodicEventPort* pe = new PeriodicEventPort(std::function<void(void)>(std::bind(&SenderImpl::periodic_event, s)), 1000);
        PeriodicEventPort* pe2 = new PeriodicEventPort(std::function<void(void)>(std::bind(&SenderImpl::periodic_event_v, s)), 1000);
    
        //Attach the Periodic Events
        s->add_event_port(pe);
        s->add_event_port(pe2);

    }else{
        std::cout << "CAN'T ADDED!!!!"  <<std::endl;
        delete s;
        s = 0;
    }
    return s;
}

void Deployment_7::startup(){
    //Construct the Component Impls
    SenderImpl* sender_impl = construct_sender_impl(this, "1");

    //Set the Attributes
    sender_impl->set_instName("OSPL_Sender");
    
    sender_impl->set_message("OSPL1");

    auto txMessage = ospl::construct_TxMessage(sender_impl, 0, "Reciever", "test_topic");
    sender_impl->_set_txMessage(txMessage);
};


