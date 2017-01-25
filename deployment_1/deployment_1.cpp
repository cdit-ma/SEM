#include "deployment_1.h"
#include <iostream>

#include <core/periodiceventport.h>
#include <core/portconfigure.h>

//Implementations for the components
#include "../components/hellosender/hellosenderimpl.h"
#include "../components/helloreceiver/helloreceiverimpl.h"

//ZMQ Implementation of the event ports
#include "../ports/message/zmq/zmqtxmessage.h"
#include "../ports/message/zmq/zmqrxmessage.h"
#include "../ports/message/qpid/qpidtxmessage.h"
#include "../ports/message/qpid/qpidrxmessage.h"
#include "../ports/message/rti/rtitxmessage.h"
#include "../ports/message/rti/rtirxmessage.h"
#include "../ports/message/ospl/ospltxmessage.h"
#include "../ports/message/ospl/osplrxmessage.h"

#include <iostream>

HelloSenderImpl* construct_sender(std::string name){
    auto sender = new HelloSenderImpl(name);
    auto pe = new PeriodicEventPort(sender, "tick", std::function<void(void)>(std::bind(&HelloSenderImpl::periodic_event, sender)), 1000);
    return sender;
}

HelloReceiverImpl* construct_receiver(std::string name){
    return new HelloReceiverImpl(name);
}

void Deployment_1::startup(){
    //Construct the Component Impls
    HelloSenderImpl* zmq_sender_impl = construct_sender("zmq_sender");
    HelloSenderImpl* qpid_sender_impl = construct_sender("qpid_sender");
    HelloSenderImpl* rti_sender_impl = construct_sender("rti_sender");
    HelloSenderImpl* ospl_sender_impl = construct_sender("ospl_sender");

    HelloReceiverImpl* qpid_receiver_impl = construct_receiver("qpid_receiver");
    HelloReceiverImpl* rti_receiver_impl = construct_receiver("rti_receiver");
    HelloReceiverImpl* ospl_receiver_impl = construct_receiver("ospl_receiver");
    

    auto zmq_txMessage = zmq::construct_TxMessage(zmq_sender_impl, "txMessage");
    auto qpid_txMessage = qpid::construct_TxMessage(qpid_sender_impl, "txMessage");
    auto rti_txMessage = rti::construct_TxMessage(rti_sender_impl, "txMessage");
    auto ospl_txMessage = ospl::construct_TxMessage(ospl_sender_impl, "txMessage");


    auto qpid_rxMessage = qpid::construct_RxMessage(qpid_receiver_impl, "rxMessage", (std::bind(&HelloReceiverImpl::rxMessage, qpid_receiver_impl, std::placeholders::_1)));
    auto rti_rxMessage = rti::construct_RxMessage(rti_receiver_impl, "rxMessage", (std::bind(&HelloReceiverImpl::rxMessage, rti_receiver_impl, std::placeholders::_1)));
    auto ospl_rxMessage = ospl::construct_RxMessage(ospl_receiver_impl, "rxMessage", (std::bind(&HelloReceiverImpl::rxMessage, ospl_receiver_impl, std::placeholders::_1)));

    zmq_sender_impl->_set_txMessage(zmq_txMessage);
    qpid_sender_impl->_set_txMessage(qpid_txMessage);
    rti_sender_impl->_set_txMessage(rti_txMessage);
    ospl_sender_impl->_set_txMessage(ospl_txMessage);

    qpid_receiver_impl->_set_rxMessage(qpid_rxMessage);
    rti_receiver_impl->_set_rxMessage(rti_rxMessage);
    ospl_receiver_impl->_set_rxMessage(ospl_rxMessage);

    add_component(zmq_sender_impl);
    add_component(qpid_sender_impl);
    add_component(rti_sender_impl);
    add_component(ospl_sender_impl);

    add_component(qpid_receiver_impl);
    add_component(rti_receiver_impl);
    add_component(ospl_receiver_impl);
};

