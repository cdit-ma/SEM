#include "deployment_9.h"

#include <iostream>

#include "common/includes/core/periodiceventport.h"

//Implementations for the components
#include "senderimpl.h"

//RTI Implementation of the event ports
#include "rti/rtitxmessage.h"
#include "opensplice/ospltxmessage.h"
#include "qpid/qpidtxmessage.h"
#include "zmq/zmqtxmessage.h"

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

void Deployment_9::startup(){
    //Construct the Component Impls
    SenderImpl* sender_impl_o = construct_sender_impl(this, "sender_impl_o");
    SenderImpl* sender_impl_r = construct_sender_impl(this, "sender_impl_r");
    SenderImpl* sender_impl_q = construct_sender_impl(this, "sender_impl_q");
    SenderImpl* sender_impl_z = construct_sender_impl(this, "sender_impl_z");

    //Set the Attributes
    sender_impl_o->set_instName("sender_impl_o");
    sender_impl_o->set_message("OSPL");

    sender_impl_r->set_instName("sender_impl_r");
    sender_impl_r->set_message("RTI");

    sender_impl_q->set_instName("sender_impl_q");
    sender_impl_q->set_message("QPID");
    
    sender_impl_z->set_instName("sender_impl_z");
    sender_impl_z->set_message("ZMQ");

    sender_impl_o->_set_txMessage(ospl::construct_TxMessage(sender_impl_o, 0, "RecieverO", "test_topic"));
    sender_impl_r->_set_txMessage(rti::construct_TxMessage(sender_impl_r, 0, "RecieverR", "test_topic_2"));
    sender_impl_q->_set_txMessage(qpid::construct_TxMessage(sender_impl_q, "localhost:5672", "a"));
    sender_impl_z->_set_txMessage(zmq::construct_TxMessage(sender_impl_z, "tcp://*:6000"));
};


