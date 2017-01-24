#include "deployment_1.h"

#include <iostream>

#include "common/includes/core/periodiceventport.h"

//Implementations for the components
#include "senderimpl.h"
#include "receiverimpl.h"

#include "proxyimpl.h"

//ZMQ Implementation of the event ports
#include "zmq/zmqtxmessage.h"
#include "zmq/zmqrxmessage.h"

#include "rti/rtitxmessage.h"
#include "rti/rtirxmessage.h"

#include <portconfigure.h>

#include "opensplice/ospltxmessage.h"
#include "opensplice/osplrxmessage.h"
#include <iostream>

void Deployment_1::startup(){
    //Construct the Component Impls
    ProxyImpl* proxy_impl = new ProxyImpl("Proxy");
    ProxyImpl* proxy_impl_loop = new ProxyImpl("LoopProxy");

    //PeriodicEventPort* pe = new PeriodicEventPort(sender_impl, "PeriodicEvent", std::function<void(void)>(std::bind(&SenderImpl::periodic_event, sender_impl)), 1000);

    //auto rxMessage = zmq::construct_RxMessage(receiver_impl, "greeting", (std::bind(&ReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)));
    //auto rxMessage2 = zmq::construct_RxMessage(receiver_impl2, "greeting", (std::bind(&ReceiverImpl::rxMessage, receiver_impl2, std::placeholders::_1)));

    auto rxMessage = qpid::construct_RxMessage(proxy_impl, "ProxyIn", (std::bind(&ProxyImpl::rxMessage, proxy_impl, std::placeholders::_1)));
    auto txMessage = qpid::construct_TxMessage(proxy_impl, "ProxyOut");

    auto rxMessageLoop = ospl::construct_RxMessage(proxy_impl, "ProxyIn", (std::bind(&ProxyImpl::rxMessage, proxy_impl, std::placeholders::_1)));
    auto txMessageLoop = zmq::construct_TxMessage(proxy_impl, "ProxyOut");
    zmq::configure_out_event_port(txMessage, "localhost:5672", "a");
    zmq::configure_in_event_port(rxMessage, "localhost:5672", "a");

    proxy_impl->_set_txMessage(txMessage);
    proxy_impl->_set_rxMessage(rxMessage);

    proxy_impl_loop->_set_txMessage(txMessageLoop);
    proxy_impl_loop->_set_rxMessage(rxMessageLoop);


    add_component(proxy_impl);
    add_component(proxy_impl_loop);

    activate_all();

};


