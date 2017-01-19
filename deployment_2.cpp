#include "deployment_2.h"

#include <iostream>

#include "common/includes/core/periodiceventport.h"

//Implementations for the components
#include "senderimpl.h"
#include "receiverimpl.h"

//ZMQ Implementation of the event ports
#include "zmq/zmqtxmessage.h"
#include "zmq/zmqrxmessage.h"

#include <iostream>

void Deployment_2::startup(){
    //Construct the Component Impls
    SenderImpl* sender_impl = new SenderImpl("Sender");
    ReceiverImpl* receiver_impl = new ReceiverImpl("Receiver");

    PeriodicEventPort* pe = new PeriodicEventPort(sender_impl, "PeriodicEvent", std::function<void(void)>(std::bind(&SenderImpl::periodic_event, sender_impl)), 1000);

    auto txMessage = zmq::construct_TxMessage(sender_impl, "greeting");
    auto rxMessage = zmq::construct_RxMessage(receiver_impl, "greeting", (std::bind(&ReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)));

    sender_impl->_set_txMessage(txMessage);
    receiver_impl->_set_rxMessage(rxMessage);

    add_component(sender_impl);
    add_component(receiver_impl);
};

