#include "registrar.h"
#include <iostream>
#include <zmq.hpp>
#include "../executionmanager.h"
#include "../../re_common/zmq/zmqutils.hpp"

//MASTER
zmq::Registrar::Registrar(ExecutionManager* manager, const std::string& endpoint){
    //Construct a context for creating sockets
    context_ = new zmq::context_t();

    endpoint_ = endpoint;
    
    //Set our execution manager    
    execution_manager_ = manager;

    //Construct a new thread for each slave
    registration_loop_ = std::async(std::launch::async, &zmq::Registrar::RegistrationLoop, this);
}

zmq::Registrar::~Registrar(){
    //Deleting the context will interupt any blocking ZMQ calls
    if(context_){
        delete context_;
    }

    registration_loop_.get();
}

void zmq::Registrar::RegistrationLoop(){
    //Construct a socket ZMQ_REP
    auto socket = zmq::socket_t(*context_, ZMQ_REP);

    //Bind
    try{
        socket.bind(endpoint_.c_str());
    }catch(const zmq::error_t& ex){
        std::cerr << "zmq::Registrar::RegistrationLoop():" << ex.what() << std::endl;
        return;
    }
    
    try{
        while(true){
            zmq::message_t zmq_request;
            zmq::message_t zmq_response;
            //Receive our message
            socket.recv(&zmq_request);
            
            const auto slave_request = Zmq2Proto<NodeManager::SlaveStartupMessage>(zmq_request);
            switch(slave_request.type()){
                case NodeManager::SlaveStartupMessage::REQUEST:{
                    //Handle a Request
                    auto slave_ip = slave_request.request().slave_ip();
                    const auto& slave_startup = execution_manager_->GetSlaveStartupMessage(slave_ip);
                    zmq_response = Proto2Zmq(slave_startup);
                    break;
                }
                case NodeManager::SlaveStartupMessage::RESPONSE:{
                    //Handle the response
                    execution_manager_->HandleSlaveResponseMessage(slave_request.response());
                    //Don't really need to send anything
                    break;
                }
                default:{
                    std::cerr << "zmq::Registrar::RegistrationLoop(): Unexceptected Slave Startup Message" << std::endl;
                    break;
                }
            }
            socket.send(zmq_response);
        }
    }catch(const zmq::error_t& ex){
        if(ex.num() != ETERM){
            std::cerr << "zmq::Registrar::RegistrationLoop():" << ex.what() << std::endl;
        }
    }
}