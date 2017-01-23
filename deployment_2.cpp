#include "deployment_2.h"

#include <iostream>

#include "common/includes/core/periodiceventport.h"

//Implementations for the components
#include "components/hellosender/hellosenderimpl.h"
#include "components/helloreceiver/helloreceiverimpl.h"

//ZMQ Implementation of the event ports
#include "ports/message/zmq/zmqtxmessage.h"
#include "ports/message/zmq/zmqrxmessage.h"

#include <iostream>

void Deployment_2::startup(){
    //Construct the Component Impls
    HelloSenderImpl* sender_impl = new HelloSenderImpl("Sender");
    HelloReceiverImpl* receiver_impl = new HelloReceiverImpl("Receiver");
    PeriodicEventPort* pe = new PeriodicEventPort(sender_impl, "PeriodicEvent", std::function<void(void)>(std::bind(&HelloSenderImpl::periodic_event, sender_impl)), 1000);

    auto txMessage = zmq::construct_TxMessage(sender_impl, "greeting");
    auto rxMessage = zmq::construct_RxMessage(receiver_impl, "greeting", (std::bind(&HelloReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)));

    sender_impl->_set_txMessage(txMessage);
    receiver_impl->_set_rxMessage(rxMessage);

    add_component(sender_impl);
    add_component(receiver_impl);
};

