#include "deployment_10.h"

//Implementations for the components
#include "receiverimpl.h"


//RTI Implementation of the event ports
#include "rti/rtirxmessage.h"
#include "opensplice/osplrxmessage.h"
#include "qpid/qpidrxmessage.h"
#include "zmq/zmqrxmessage.h"

void Deployment_10::startup(){
        //Construct the Component Impls
        
        ReceiverImpl* receiver_impl_o = new ReceiverImpl("receiver_impl_o");
        ReceiverImpl* receiver_impl_r = new ReceiverImpl("receiver_impl_r");
        ReceiverImpl* receiver_impl_q = new ReceiverImpl("receiver_impl_q");
        ReceiverImpl* receiver_impl_z = new ReceiverImpl("receiver_impl_z");


        //Set the Attributes
        receiver_impl_o->set_instName("receiver_impl_o");
        receiver_impl_r->set_instName("receiver_impl_r");
        receiver_impl_q->set_instName("receiver_impl_q");
        receiver_impl_z->set_instName("receiver_impl_z");
        
        receiver_impl_o->_set_rxMessage(ospl::construct_RxMessage(receiver_impl_o, (std::bind(&ReceiverImpl::rxMessage, receiver_impl_o, std::placeholders::_1)), 0, "subscriber", "test_topic_2"));
        receiver_impl_r->_set_rxMessage(rti::construct_RxMessage(receiver_impl_r, (std::bind(&ReceiverImpl::rxMessage, receiver_impl_r, std::placeholders::_1)), 0, "subscriber2", "test_topic"));
        receiver_impl_q->_set_rxMessage(qpid::construct_RxMessage(receiver_impl_q, (std::bind(&ReceiverImpl::rxMessage, receiver_impl_q, std::placeholders::_1)), "localhost:5672", "a"));
        receiver_impl_z->_set_rxMessage(zmq::construct_RxMessage(receiver_impl_z, (std::bind(&ReceiverImpl::rxMessage, receiver_impl_z, std::placeholders::_1)), "tcp://192.168.111.84:6000"));

        //Add Components to Container
        add_component(receiver_impl_o);
        add_component(receiver_impl_r);
        add_component(receiver_impl_q);
        add_component(receiver_impl_z);
};