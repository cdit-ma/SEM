#include "deployment_2.h"

//Implementations for the components
#include "receiverimpl.h"


//ZMQ Implementation of the event ports
#include "zmq/zmqrxmessage.h"
//#include "zmq/zmqrxvectormessage.h"

void Deployment_2::startup(){
        //Construct the Component Impls
        ReceiverImpl* receiver_impl = new ReceiverImpl("Receiver");
       // ReceiverImpl* receiver_impl2 = new ReceiverImpl("receiver_impl2");

        //Set the Attributes
        receiver_impl->set_instName("ZMQ_Receiver 1");
       // receiver_impl2->set_instName("ZMQ_Receiver 2");
        
        auto rxMessage = zmq::construct_RxMessage(receiver_impl, "greeting", (std::bind(&ReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)));
        
        rxMessage->set_name("greeting");
        receiver_impl->_set_rxMessage(rxMessage);
        //receiver_impl2->_set_rxVectorMessage(rxVectorMessage);

        //Add Components to Container
        add_component(receiver_impl);
};