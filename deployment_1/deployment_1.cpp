#include "deployment_1.h"

#include <iostream>
#include "common/includes/core/portconfigure.h"

#include <core/periodiceventport.h>

//Implementations for the components
#include "../components/hellosender/hellosenderimpl.h"
#include "../components/helloreceiver/helloreceiverimpl.h"

//ZMQ Implementation of the event ports
#include "ports/message/zmq/zmqtxmessage.h"
#include "ports/message/zmq/zmqrxmessage.h"

#include <iostream>

void Deployment_1::startup(){
    //Construct the Component Impls
    HelloSenderImpl* sender_impl = new HelloSenderImpl("sender");
    PeriodicEventPort* pe = new PeriodicEventPort(sender_impl, "PeriodicEvent", std::function<void(void)>(std::bind(&HelloSenderImpl::periodic_event, sender_impl)), 1000);

    auto txMessage = zmq::construct_TxMessage(sender_impl, "txMessage");

    sender_impl->_set_txMessage(txMessage);


    add_component(sender_impl);
};

