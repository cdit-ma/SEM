#include "managedserver.h"
#include <re_common/proto/controlmessage/controlmessage.pb.h>
#include <zmq.hpp>

ManagedServer::ManagedServer(Execution& execution, const std::string& address, const std::string& experiment_id, const std::string& environment_manager_address) : execution_(execution)
{
    std::string default_database_file_name = experiment_id + ".sql";

    address_ = address;
    experiment_id_ = experiment_id;
    environment_manager_address_ = environment_manager_address;

    requester_ = std::unique_ptr<EnvironmentRequester>(new EnvironmentRequester(environment_manager_address_, experiment_id_, EnvironmentRequester::DeploymentType::LOGAN));

    requester_->AddUpdateCallback(std::bind(&ManagedServer::HandleUpdate, this, std::placeholders::_1));
    requester_->Init(environment_manager_address_);
    requester_->SetIPAddress(address);
    requester_->Start();


    auto retry_count = 0;

    NodeManager::EnvironmentMessage message; 
    bool got_response = false;
    while(true){
        try{
            message = requester_->GetLoganInfo(address);

            if(message.type() == NodeManager::EnvironmentMessage::LOGAN_RESPONSE){
                break;
            }else if(message.type() == NodeManager::EnvironmentMessage::LOGAN_QUERY){
                got_response = true;
            }
        }catch(const std::exception& ex){}
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        if(retry_count++ >= MAX_RETRY_COUNT){
            if(got_response){
                throw std::runtime_error("No experiment alive requires Logan Server.");
            }else{
                throw std::runtime_error("Couldn't communicate with Environment Manager.");
            }
        }
    }

    for(const auto logger : message.logger()){
        if(logger.type() == NodeManager::Logger::SERVER){
            std::vector<std::string> client_list;
            for(const auto& address : logger.client_addresses()){
                client_list.push_back(address);
            }
            servers_.push_back(std::move(std::unique_ptr<Server>(new Server(logger.db_file_name(), client_list))));
            servers_.back()->Start();
        }
    }

    if(!servers_.size()){
        //Interupt the execution
        execution_.Interrupt();
    }
}

void ManagedServer::Terminate(){
    
    servers_.clear();
    execution_.Interrupt();
}

void ManagedServer::HandleUpdate(NodeManager::EnvironmentMessage& message){
    switch(message.type()){
        case NodeManager::EnvironmentMessage::ERROR_RESPONSE:{
            Terminate();
            break;
        }
        case NodeManager::EnvironmentMessage::UPDATE_DEPLOYMENT:{
            break;
        }
        default:{
            Terminate();
        }
    }
}
