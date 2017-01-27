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
#include <map>

std::map<std::string, std::function<Component* (std::string)> > constructor_map_; 


Deployment_2::Deployment_2(){
}

EventPort* Deployment_2::construct_rx(std::string middleware, std::string datatype, Component* component, std::string port_name){
    EventPort* p = 0;

    if(middleware == "zmq"){
        p = zmq::construct_rx(datatype, component, port_name);
    }
    if(p){
        std::cout << "construct_tx(" << middleware << ", " << datatype << ", " << port_name << "): " << p << std::endl;
    }
    return p;
};

EventPort* Deployment_2::construct_tx(std::string middleware, std::string datatype, Component* component, std::string port_name){
    EventPort* p = 0;
    if(middleware == "zmq"){
        p = zmq::construct_tx(datatype, component, port_name);
    }
    if(p){
        std::cout << "construct_tx(" << middleware << ", " << datatype << ", " << port_name << "): " << p << std::endl;
    }
    return p;
}

Component* Deployment_2::construct_component(std::string component_type, std::string component_name){
    Component* c = 0;
    if(component_type == "HelloSender"){
        c = new HelloSenderImpl(component_name);
    }else if(component_type== "HelloReceiver"){
        c = new HelloReceiverImpl(component_name);
    }
    if(c){
        add_component(c);
        std::cout << "construct_component(" << component_type << ", " << component_name << "): " << c << std::endl;
    }
    return c;
}

void Deployment_2::startup(){
    /*
    //Construct the Component Impls
    //HelloSenderImpl* sender_impl = new HelloSenderImpl("zmq_sender");
    //HelloReceiverImpl* receiver_impl = new HelloReceiverImpl("zmq_receiver");
    
    //HELLO
    
    auto p_e = new PeriodicEventPort(sender_impl, "tick", sender_impl->get_callback("periodic_event"), 1000);
    auto txMessage = zmq::construct_tx("Message", sender_impl, "txMessage");
    auto rxMessage = zmq::construct_rx("Message", receiver_impl, "rxMessage");
    std::cout << txMessage << std::endl;
    std::cout << rxMessage << std::endl;

    add_component(sender_impl);
    add_component(receiver_impl);*/
};

