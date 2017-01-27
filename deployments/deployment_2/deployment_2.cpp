#include "deployment_2.h"

#include <iostream>
#include <core/periodiceventport.h>

//Implementations for the components
#include "../../components/hellosender/hellosenderimpl.h"
#include "../../components/helloreceiver/helloreceiverimpl.h"

//ZMQ Implementation of the event ports
//#include "../../middleware/zmq/message/tx.h"
//#include "../../middleware/zmq/message/rx.h"
#include "../../middleware/zmq/test.h"

#include <iostream>

void Deployment_2::startup(){
    //Construct the Component Impls
    HelloReceiverImpl* receiver_impl = new HelloReceiverImpl("zmq_receiver");

    auto rxMessage = zmq::construct_rx("Message", receiver_impl, "rxMessage", (std::bind(&HelloReceiverImpl::test, receiver_impl, std::placeholders::_1)));
    std::cout << rxMessage << std::endl;
    //receiver_impl->_set_rxMessage(rxMessage);

    add_component(receiver_impl);
};

