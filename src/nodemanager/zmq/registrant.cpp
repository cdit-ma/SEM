#include "registrant.h"
#include <core/modellogger.h>

#include <zmq.hpp>
#include <iostream>
#include <chrono>
#include "../../re_common/zmq/zmqutils.hpp"
#include "../deploymentmanager.h"

//SLAVE
zmq::Registrant::Registrant(DeploymentManager& deployment_manager):
    deployment_manager_(deployment_manager)
{
    //Construct context
    context_ = new zmq::context_t(1);

    //Start an async registration loop
    registration_loop_ = std::async(std::launch::async, &zmq::Registrant::RegistrationLoop, this);
}

zmq::Registrant::~Registrant(){
    std::lock_guard<std::mutex> lock(mutex_);
    
    if(context_){
        delete context_;
        context_ = 0;
    }

    registration_loop_.get();
    deployment_manager_.Teardown();
}

//CLient
void zmq::Registrant::RegistrationLoop(){
    //Start a request socket, and bind endpoint
    auto socket = zmq::socket_t(*context_, ZMQ_REQ);

    try{
        if(!deployment_manager_.QueryEnvironmentManager()){
            return;
        }

        //Get the Registrant Port
        auto slave_ip_address = deployment_manager_.GetSlaveIPAddress();
        auto master_registration_endpoint = deployment_manager_.GetMasterRegistrationEndpoint();

        std::cerr << "Connecting Master Registration Endpoint: " << master_registration_endpoint << std::endl;
        socket.connect(master_registration_endpoint.c_str());

        {
            //Send our NodeManager::SlaveStartupMessage
            NodeManager::SlaveStartupMessage slave_request;
            slave_request.set_type(NodeManager::SlaveStartupMessage::REQUEST);
            slave_request.mutable_request()->set_slave_ip(slave_ip_address);
            socket.send(Proto2Zmq(slave_request));
            std::cerr << "HAVE WE SENT" << std::endl;
        }
        {
            //Recieve the startup request
            zmq::message_t zmq_slave_startup;
            socket.recv(&zmq_slave_startup);

            //std::cerr << zmq_slave_startup.data() << std::endl;
            
            std::cerr << "HELLO1" << std::endl;
            auto slave_startup = Zmq2Proto<NodeManager::SlaveStartupMessage>(zmq_slave_startup);


            std::cerr << slave_startup.DebugString() << std::endl;

            if(slave_startup.type() == NodeManager::SlaveStartupMessage::STARTUP){
                auto slave_response = deployment_manager_.HandleSlaveStartup(slave_startup.startup());

                //Send our NodeManager::StartupResponse
                NodeManager::SlaveStartupMessage response;
                response.set_type(NodeManager::SlaveStartupMessage::RESPONSE);
                response.set_allocated_response(&slave_response);
                socket.send(Proto2Zmq(response));
            }else{
                return;
            }
        }

        {
            //Recieve nothing
            zmq::message_t zmq_master_response;
            socket.recv(&zmq_master_response);
        }
    }catch(const zmq::error_t& ex){
        if(ex.num() != ETERM){
            std::cerr << "zmq::Registrant::RegistrationLoop():" << ex.what() << std::endl;
        }
    }catch(const std::exception& e){
        std::cerr << "zmq::Registrant::RegistrationLoop():" << e.what() << std::endl;
    }
}