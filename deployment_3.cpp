#include "deployment_3.h"

#include <iostream>

#include "common/includes/core/periodiceventport.h"

//Implementations for the components
#include "senderimpl.h"

//ZMQ Implementation of the event ports
#include "qpid/qpidtxmessage.h"
#include "qpid/qpidtxvectormessage.h"

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

void Deployment_1::startup(){
    //Construct the Component Impls
    SenderImpl* sender_impl = construct_sender_impl(this, "1");
    SenderImpl* sender_impl2 = construct_sender_impl(this, "2");

    //Set the Attributes
    sender_impl->set_instName("ZMQ_Sender");
    sender_impl2->set_instName("ZMQ_Sender2");
    
    sender_impl->set_message("ZMQ1");
    sender_impl2->set_message("ZMQ2");

    auto txMessage = qpid::construct_TxMessage(sender_impl, std::string("tcp://*:6000"));
    auto txVectorMessage = qpid::construct_TxVectorMessage(sender_impl2, std::string("tcp://*:6001"));
    sender_impl->_set_txMessage(txMessage);
    sender_impl2->_set_txVectorMessage(txVectorMessage);
};


