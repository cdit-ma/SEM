#include "deployment_1.h"

//Interfaces for the Components
#include "interfaces.h"

//Implementations for the components
#include "senderimpl.h"

//ZMQ Implementation of the event ports
#include "zmq/zmqtxmessage.h"
#include "zmq/zmqtxvectormessage.h"

SenderImpl* construct_sender_impl(NodeContainer* c, std::string name){
    SenderImpl* s = new SenderImpl(name);
    if(c->add_component(s)){
        //Do specifics

        PeriodicEvent* pe = new PeriodicEvent(std::function<void(void)>(std::bind(&SenderImpl::periodic_event, s)), 1000);
        PeriodicEvent* pe2 = new PeriodicEvent(std::function<void(void)>(std::bind(&SenderImpl::periodic_event_v, s)), 1000);
    
        //Attach the Periodic Events
        s->add_event_port(pe);
        s->add_event_port(pe2);

    }else{
        std::cout << "CAN'T ADDED!!!!"  <<std::endl;
        delete s;
        s = 0;
    }
    return s;
}

void Deployment_1::startup(){
    //Construct the Component Impls
    
    SenderImpl* sender_impl = construct_sender_impl(this, "ComponentAssembly::Sender1");
    SenderImpl* sender_impl2 = construct_sender_impl(this, "ComponentAssembly::Sender2");

    //Set the Attributes
    sender_impl->set_instName("ZMQ_Sender");
    sender_impl2->set_instName("ZMQ_Sender2");
    
    sender_impl->set_message("ZMQ1");
    sender_impl2->set_message("ZMQ2");

    //Construct the ports
    txMessageInt* zmq_tx = new zmq::TxMessage(sender_impl, std::string("tcp://*:6000"));
    txVectorMessageInt* zmq_v_tx = new zmq::TxVectorMessage(sender_impl2, std::string("tcp://*:6001"));

    //Attach the ports
    sender_impl->_set_txMessage(zmq_tx);
    sender_impl2->_set_txMessage(0);
    sender_impl->_set_txVectorMessage(0);
    sender_impl2->_set_txVectorMessage(zmq_v_tx);

};


