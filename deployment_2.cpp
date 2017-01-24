#include "deployment_2.h"

#include <iostream>
#include <core/periodiceventport.h>

//Implementations for the components
#include "components/hellosender/hellosenderimpl.h"
#include "components/helloreceiver/helloreceiverimpl.h"

//ZMQ Implementation of the event ports
#include "ports/message/zmq/zmqtxmessage.h"
#include "ports/message/zmq/zmqrxmessage.h"

#include <iostream>

void Deployment_2::startup(){
    //Construct the Component Impls
    HelloReceiverImpl* receiver_impl = new HelloReceiverImpl("Receiver");

    auto rxMessage = zmq::construct_RxMessage(receiver_impl, "rxMessage", (std::bind(&HelloReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)));

    receiver_impl->_set_rxMessage(rxMessage);

    add_component(receiver_impl);
};

