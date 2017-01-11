#include "deployment_1.h"

//Interfaces for the Components
#include "interfaces.h"

//Implementations for the components
#include "senderimpl.h"

//ZMQ Implementation of the event ports
#include "zmq/zmqtxmessage.h"
#include "zmq/zmqtxvectormessage.h"

void Deployment_1::startup(){

    //Construct the Component Impls
    SenderImpl* sender_impl = new SenderImpl("sender_impl");
    SenderImpl* sender_impl2 = new SenderImpl("sender_impl2");

    //Set the Attributes
    sender_impl->set_instName("ZMQ_Sender");
    sender_impl2->set_instName("ZMQ_Sender2");
    
    sender_impl->set_message("ZMQ1");
    sender_impl2->set_message("ZMQ2");

    //Construct the ports
    txMessageInt* zmq_tx = new zmq::TxMessage(sender_impl, std::string("tcp://*:6000"));
    txVectorMessageInt* zmq_v_tx = new zmq::TxVectorMessage(sender_impl2, std::string("tcp://*:6001"));
    
    //Construct Periodic Events
    PeriodicEvent* pe = new PeriodicEvent(std::function<void(void)>(std::bind(&SenderImpl::periodic_event, sender_impl)), 1000);
    PeriodicEvent* pe2 = new PeriodicEvent(std::function<void(void)>(std::bind(&SenderImpl::periodic_event_v, sender_impl2)), 1000);
    
    //Attach the Periodic Events
    sender_impl->add_event_port(pe);
    sender_impl2->add_event_port(pe2);

    //Attach the ports
    sender_impl->_set_txMessage(zmq_tx);
    sender_impl->_set_txVectorMessage(zmq_v_tx);
    
    //Add Components to Container
    add_component(sender_impl);
    add_component(sender_impl2);
};


