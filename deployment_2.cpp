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

    PeriodicEventPort* pe = new PeriodicEventPort(sender_impl, "PeriodicEvent", std::function<void(void)>(std::bind(&SenderImpl::periodic_event, sender_impl)), 1000);

    //Construct our TX MEssage
    auto txMessage = zmq::construct_TxMessage(sender_impl, "greeting");
    sender_impl->_set_txMessage(txMessage);

    add_component(sender_impl);
};

