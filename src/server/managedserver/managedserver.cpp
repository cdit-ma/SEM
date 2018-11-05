#include "managedserver.h"
#include <proto/controlmessage/controlmessage.pb.h>
#include <zmq.hpp>
#include <iostream>

ManagedServer::ManagedServer(Execution& execution, const std::string& experiment_name, const std::string& ip_address, const std::string& environment_manager_endpoint)
:
execution_(execution),
experiment_name_(experiment_name),
ip_address_(ip_address)
{
    execution_.AddTerminateCallback(std::bind(&ManagedServer::Terminate, this));

    std::unique_ptr<NodeManager::LoganRegistrationReply> logan_info;
    try{
        logan_info = EnvironmentRequest::TryRegisterLoganServer(environment_manager_endpoint, experiment_name_, ip_address_);
    }catch(const std::exception& ex){
        std::cerr << "* Failed to Register with EnvironmentManager: " << ex.what() << std::endl;
        throw;
    }

    for(const auto& logger : logan_info->logger()){
        if(logger.type() == NodeManager::Logger::SERVER){
            std::vector<std::string> client_list;
            for(const auto& address : logger.client_addresses()){
                client_list.push_back(address);
            }
            servers_.push_back(std::move(std::unique_ptr<Server>(new Server(logger.db_file_name(), client_list))));
        }
    }

    if(!servers_.size()){
        throw NotNeededException("Not Needed");
    }

    //Construct a heartbeater
    requester_ = std::unique_ptr<EnvironmentRequest::HeartbeatRequester>(new EnvironmentRequest::HeartbeatRequester(logan_info->heartbeat_endpoint(), std::bind(&ManagedServer::HandleExperimentUpdate, this, std::placeholders::_1)));
    requester_->SetTimeoutCallback(std::bind(&ManagedServer::DelayedTerminate, this));
}

void ManagedServer::Terminate(){
    auto start = std::chrono::steady_clock::now();
    servers_.clear();
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Termination took: " << ms.count() << " ms" << std::endl;
    execution_.Interrupt();
    if(requester_){
        requester_.reset();
    }
}
void ManagedServer::DelayedTerminate(){
    std::this_thread::sleep_for(std::chrono::seconds(20));
    Terminate();
}

void ManagedServer::HandleExperimentUpdate(NodeManager::EnvironmentMessage& message){
    switch(message.type()){
        case NodeManager::EnvironmentMessage::SHUTDOWN_LOGAN_SERVER:{
            Terminate();
            break;
        }
        default:{
            break;
        }
    }
}
