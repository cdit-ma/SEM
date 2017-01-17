#include "deployment_6.h"

//Implementations for the components
#include "receiverimpl.h"


//RTI Implementation of the event ports
#include "rti/rtirxmessage.h"

void Deployment_6::startup(){
        //Construct the Component Impls
        ReceiverImpl* receiver_impl = new ReceiverImpl("receiver_impl");

        //Set the Attributes
        receiver_impl->set_instName("RTI_Receiver 1");
        
        auto rxMessage = rti::construct_RxMessage(receiver_impl, (std::bind(&ReceiverImpl::rxMessage, receiver_impl, std::placeholders::_1)), 0, "publisher", "test_topic");
        receiver_impl->_set_rxMessage(rxMessage);

        //Add Components to Container
        add_component(receiver_impl);
};