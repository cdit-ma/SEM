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
}

//CLient
void zmq::Registrant::RegistrationLoop(){
    //Start a request socket, and bind endpoint
    auto socket = zmq::socket_t(*context_, ZMQ_REQ);

    try{
        std::cerr << "deployment_manager_.QueryEnvironmentManager()" << std::endl;
        auto query_success = deployment_manager_.QueryEnvironmentManager();
        std::cerr << "query_success: " << (query_success ? " TRUE " : " FALSE ") << std::endl;

        if(!query_success){
            std::cerr << "deployment_manager_.Teardown()" << std::endl;
            deployment_manager_.Teardown();
            std::cerr << "deployment_manager_.Teardown()d" << std::endl;
            return;
        }

        //Get the Registrant Port
        auto slave_ip_address = deployment_manager_.GetSlaveIPAddress();
        auto master_registration_endpoint = deployment_manager_.GetMasterRegistrationEndpoint();

        socket.connect(master_registration_endpoint.c_str());

        {
            //Send our NodeManager::SlaveStartupMessage
            NodeManager::SlaveStartupMessage slave_request;
            slave_request.set_type(NodeManager::SlaveStartupMessage::REQUEST);
            slave_request.mutable_request()->set_slave_ip(slave_ip_address);
            socket.send(Proto2Zmq(slave_request));
        }
        {
            //Recieve the startup request
            zmq::message_t zmq_slave_startup;
            socket.recv(&zmq_slave_startup);

            auto slave_startup = Zmq2Proto<NodeManager::SlaveStartupMessage>(zmq_slave_startup);

            if(slave_startup.type() == NodeManager::SlaveStartupMessage::STARTUP){
                const auto& slave_response = deployment_manager_.HandleSlaveStartup(slave_startup.startup());

                //Send our NodeManager::StartupResponse
                NodeManager::SlaveStartupMessage response;
                response.set_type(NodeManager::SlaveStartupMessage::RESPONSE);
                response.set_allocated_response(new NodeManager::SlaveStartupResponse(slave_response));
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