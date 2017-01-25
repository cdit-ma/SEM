#include "deployment_2.h"

#include <iostream>
#include <core/periodiceventport.h>

#include <core/basemessage.h>
//Implementations for the components
#include "../components/hellosender/hellosenderimpl.h"
#include "../components/helloreceiver/helloreceiverimpl.h"

//ZMQ Implementation of the event ports
#include "../ports/message/zmq/zmqtxmessage.h"
#include "../ports/message/zmq/zmqrxmessage.h"

#include <iostream>

void test_derived(Message* test){

};

void test_base(BaseMessage* test){

};

void Deployment_2::startup(){
    //Construct the Component Impls
    HelloReceiverImpl* receiver_impl = new HelloReceiverImpl("zmq_receiver");
    HelloSenderImpl* sender_impl = new HelloSenderImpl("zmq_sender");

    auto pe = new PeriodicEventPort(sender_impl, "periodic_event", sender_impl->get_callback("periodic_event"), 1000);

    
    auto rxMessage = zmq::construct_RxMessage(receiver_impl, "rxMessage", receiver_impl->get_callback("rxMessage"));
    auto txMessage = zmq::construct_TxMessage(sender_impl, "txMessage");

    //receiver_impl->_set_rxMessage(rxMessage);
    //sender_impl->_set_txMessage(txMessage);

    add_component(receiver_impl);
    add_component(sender_impl);
};

