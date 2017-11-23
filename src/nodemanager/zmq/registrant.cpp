#include "registrant.h"
#include <core/modellogger.h>

#include <zmq.hpp>
#include <iostream>
#include <chrono>
#include "../../re_common/zmqutils.hpp"

zmq::Registrant::Registrant(DeploymentManager* manager, std::string endpoint){
    deployment_manager_ = manager;
    
    //Construct context
    context_ = new zmq::context_t(1);


    
    //Start the registration thread
    registration_thread_ = new std::thread(&zmq::Registrant::RegistrationLoop, this, endpoint);
}

zmq::Registrant::~Registrant(){
    //Deleting the context will interupt any blocking ZMQ calls
    
    if(context_){
        delete context_;    
    }

    if(registration_thread_){
        registration_thread_->join();
        delete registration_thread_;
    }

    if(deployment_manager_){
        deployment_manager_->TeardownModelLogger();
        delete deployment_manager_;
    }
}

void zmq::Registrant::RegistrationLoop(std::string endpoint){
    //Start a request socket, and bind endpoint
    auto socket = zmq::socket_t(*context_, ZMQ_PAIR);
    socket.bind(endpoint.c_str());

    try{
        zmq::message_t slave_startup;

        //Send our slave address
        socket.send(zmq::message_t(endpoint.begin(), endpoint.end()));

        //Recieve the startup request
        socket.recv(&slave_startup);
        NodeManager::Startup slave_startup_pb;
        slave_startup_pb.ParseFromArray(slave_startup.data(), slave_startup.size());
        auto slave_response = deployment_manager_->HandleStartup(slave_startup_pb);

        //Send the slave response
        socket.send(Proto2Zmq(slave_response));
    }catch(const zmq::error_t& ex){
        if(ex.num() != ETERM){
            std::cerr << "zmq::Registrant::RegistrationLoop():" << ex.what() << std::endl;
        }
    }
}