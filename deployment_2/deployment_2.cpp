#include "deployment_2.h"

#include <iostream>
#include <core/periodiceventport.h>

//Implementations for the components
#include "../components/hellosender/hellosenderimpl.h"
#include "../components/helloreceiver/helloreceiverimpl.h"

//ZMQ Implementation of the event ports
#include "../ports/message/qpid/qpidtxmessage.h"
#include "../ports/message/qpid/qpidrxmessage.h"

#include <iostream>

void Deployment_2::startup(){
    //Construct the Component Impls
    HelloReceiverImpl* receiver_impl = new HelloReceiverImpl("zmq_receiver");

    auto rxMessage = qpid::construct_RxMessage(receiver_impl, "rxMessage", (std::bind(&HelloReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)));

    receiver_impl->_set_rxMessage(rxMessage);

    add_component(receiver_impl);
};

