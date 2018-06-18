#include "registrar.h"
#include <iostream>
#include <zmq.hpp>
#include "../executionmanager.h"
#include "../../re_common/zmq/zmqutils.hpp"

zmq::Registrar::Registrar(ExecutionManager* manager, const std::string& publisher_endpoint){
    //Construct a context for creating sockets
    context_ = new zmq::context_t();

    //Set the publisher_endpoint which we will send to the Registrants
    publisher_endpoint_ = publisher_endpoint;
    
    //Set our execution manager    
    execution_manager_ = manager;

    //Construct a new thread for each slave
    for(const auto& s : execution_manager_->GetSlaveAddresses()){
        //Adding std::launch::async forces the async process to start instantaneously. 
        registration_results_.emplace_back(std::async(std::launch::async, &zmq::Registrar::RegistrationLoop, this, s));
    }
}

zmq::Registrar::~Registrar(){
    //Deleting the context will interupt any blocking ZMQ calls
    if(context_){
        delete context_;
    }

    //Join all registration threads
    while(!registration_results_.empty()){
        auto &t = registration_results_.back();
        t.get();
        registration_results_.pop_back();
    }
}

bool zmq::Registrar::RegistrationLoop(const std::string& endpoint){
    //Construct a socket (Using Pair)
    try{
        auto socket = zmq::socket_t(*context_, ZMQ_REP);
        //Connect to the socket
        socket.connect(endpoint.c_str()); 
        zmq::message_t slave_addr;
        zmq::message_t slave_response;
        
        //Wait for a slave address
        socket.recv(&slave_addr);
        const auto& slave_addr_str = zmq::Zmq2String(slave_addr);

        //Send the startup response
        const auto& slave_startup_pb = execution_manager_->GetSlaveStartupMessage(slave_addr_str);
        socket.send(Proto2Zmq(slave_startup_pb));

        //Wait for Slave to send a message
        socket.recv(&slave_response);
        NodeManager::StartupResponse slave_response_pb;
        slave_response_pb.ParseFromArray(slave_response.data(), slave_response.size());
        execution_manager_->HandleSlaveResponseMessage(slave_addr_str, slave_response_pb);
        socket.send(Proto2Zmq(slave_startup_pb));
        return true;
    }catch(const zmq::error_t& ex){
        if(ex.num() != ETERM){
            std::cerr << "zmq::Registrar::RegistrationLoop():" << ex.what() << std::endl;
        }
    }
    return false;
}