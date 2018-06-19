#include "registrar.h"
#include <iostream>
#include <zmq.hpp>
#include "../executionmanager.h"
#include "../../re_common/zmq/zmqutils.hpp"

//MASTER
zmq::Registrar::Registrar(ExecutionManager& execution_manager):
    execution_manager_(execution_manager)
{
    //Construct a context for creating sockets
    context_ = new zmq::context_t();

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

    auto endpoint = execution_manager_.GetMasterRegistrationEndpoint();

    //Bind
    try{
        socket.bind(endpoint.c_str());
    }catch(const zmq::error_t& ex){
        std::cerr << "zmq::Registrar::RegistrationLoop(): Faield to Bind: " << endpoint << " " << ex.what() << std::endl;
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
                    NodeManager::SlaveStartupMessage slave_startup;
                    slave_startup.set_type(NodeManager::SlaveStartupMessage::STARTUP);

                    //Handle a Request
                    auto slave_ip = slave_request.request().slave_ip();
                    const auto slave_startup_ = execution_manager_.GetSlaveStartupMessage(slave_ip);

                    slave_startup.set_allocated_startup(new NodeManager::SlaveStartup(slave_startup_));
                    zmq_response = Proto2Zmq(slave_startup);
                    break;
                }
                case NodeManager::SlaveStartupMessage::RESPONSE:{
                    //Handle the response
                    execution_manager_.HandleSlaveResponseMessage(slave_request.response());
                    //Don't really need to send anything
                    break;
                }
                default:{
                    std::cerr << "zmq::Registrar::RegistrationLoop(): Unexceptected Slave Startup Message" << std::endl;
                    break;
                }
            }
            std::cerr << "Registrar: SENDING RESPONSE " << std::endl;
            std::cerr << zmq_response.size() << std::endl;
            socket.send(zmq_response);
        }
    }catch(const zmq::error_t& ex){
        if(ex.num() != ETERM){
            std::cerr << "zmq::Registrar::RegistrationLoop():" << ex.what() << std::endl;
        }
    }
}