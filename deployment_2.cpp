#include "deployment_2.h"

//Interfaces for the Components
#include "interfaces.h"

//Implementations for the components
#include "receiverimpl.h"

//ZMQ Implementation of the event ports
#include "zmq/zmqrxmessage.h"
#include "zmq/zmqrxvectormessage.h"

void Deployment_2::startup(){
        //Construct the Component Impls
        ReceiverImpl* receiver_impl = new ReceiverImpl("receiver_impl");
        ReceiverImpl* receiver_impl2 = new ReceiverImpl("receiver_impl2");

        //Set the Attributes
        receiver_impl->set_instName("ZMQ_Receiver 1");
        receiver_impl2->set_instName("ZMQ_Receiver 2");
        
        //Construct the ports
        rxMessageInt* zmq_rx = new zmq::RxMessage(receiver_impl, std::string("tcp://localhost:6000"));;
        rxVectorMessageInt* zmq_v_rx = new zmq::RxVectorMessage(receiver_impl2, std::string("tcp://localhost:6001"));;
     
        //Attach the ports
        receiver_impl->_set_rxMessage(zmq_rx);
        receiver_impl2->_set_rxVectorMessage(zmq_v_rx);
        
        //Add Components to Container
        add_component(receiver_impl);
        add_component(receiver_impl2);
};