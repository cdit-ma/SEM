#include "deployment_2.h"

#include <iostream>
#include "common/includes/core/portconfigure.h"

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
    HelloSenderImpl* sender_impl = new HelloSenderImpl("Sender");
    HelloReceiverImpl* receiver_impl = new HelloReceiverImpl("Receiver");
    PeriodicEventPort* pe = new PeriodicEventPort(sender_impl, "PeriodicEvent", std::function<void(void)>(std::bind(&HelloSenderImpl::periodic_event, sender_impl)), 1000);

    auto txMessage = zmq::construct_TxMessage(sender_impl, "greeting");
    auto rxMessage = zmq::construct_RxMessage(receiver_impl, "greeting", (std::bind(&HelloReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)));

    std::vector<std::string> endpoints;
    endpoints.push_back("tcp://192.168.111.84:6001");

    zmq::configure_out_event_port(txMessage, "tcp://192.168.111.84:6001");
    zmq::configure_in_event_port(rxMessage, endpoints);

    sender_impl->_set_txMessage(txMessage);
    receiver_impl->_set_rxMessage(rxMessage);

    sender_impl->set_message("MANUAL MESSAGE");
    sender_impl->set_instName("Sender");

    add_component(sender_impl);
    add_component(receiver_impl);
    activate_all();

};

