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
    ReceiverImpl* receiver_impl = new ReceiverImpl("Receiver");


    //Construct our TX MEssage
    auto rxMessage = zmq::construct_RxMessage(proxy_impl, "greeting", (std::bind(&ReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)));

    receiver_impl->_set_rxMessage(rxMessage);

    add_component(receiver_impl);
};

