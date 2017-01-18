#include "deployment_4.h"

//Implementations for the components
#include "receiverimpl.h"


//ZMQ Implementation of the event ports
#include "qpid/qpidrxmessage.h"
#include "qpid/qpidrxvectormessage.h"

void Deployment_4::startup(){
        //Construct the Component Impls
        ReceiverImpl* receiver_impl = new ReceiverImpl("receiver_impl");
        ReceiverImpl* receiver_impl2 = new ReceiverImpl("receiver_impl2");

        //Set the Attributes
        receiver_impl->set_instName("QPID_Receiver 1");
        receiver_impl2->set_instName("QPID_Receiver 2");
        
        auto rxMessage = qpid::construct_RxMessage(receiver_impl, (std::bind(&ReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)), "localhost:5672", "a");
        auto rxVectorMessage = qpid::construct_RxVectorMessage(receiver_impl2, (std::bind(&ReceiverImpl::rxVectorMessage, receiver_impl2, std::placeholders::_1)),"localhost:5672", "b");
        receiver_impl->_set_rxMessage(rxMessage);
        receiver_impl2->_set_rxVectorMessage(rxVectorMessage);

        //Add Components to Container
        add_component(receiver_impl);
        add_component(receiver_impl2);
};