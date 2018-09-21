#include "environmentrequester.h"
#include <zmq.hpp>

#include <proto/controlmessage/controlmessage.pb.h>
#include <zmq/protorequester/protorequester.hpp>
#include <zmq/zmqutils.hpp>
#include <sstream>

EnvironmentRequester::EnvironmentRequester(const std::string& manager_address, 
                                            const std::string& experiment_id,
                                            EnvironmentRequester::DeploymentType deployment_type) : 
                                            manager_address_(manager_address),
                                            experiment_id_(experiment_id),
                                            deployment_type_(deployment_type)
{}

EnvironmentRequester::~EnvironmentRequester(){
    Terminate();
}

void EnvironmentRequester::Init(const std::string& manager_endpoint){
    manager_endpoint_ = manager_endpoint;
}

void EnvironmentRequester::AddUpdateCallback(std::function<void (NodeManager::EnvironmentMessage& environment_message)> callback_func){
    update_callback_ = callback_func;
}

void EnvironmentRequester::SetIPAddress(const std::string& ip_addr){
    ip_address_ = ip_addr;
}

NodeManager::ControlMessage EnvironmentRequester::NodeQuery(const std::string& node_endpoint){
    //Construct query message
    NodeManager::EnvironmentMessage message;
    message.set_experiment_id(experiment_id_);
    message.set_type(NodeManager::EnvironmentMessage::NODE_QUERY);

    auto control_message = message.mutable_control_message();

    auto node = control_message->add_nodes();
    auto info = node->mutable_info();

    auto& attribute = (*node->mutable_attributes())["ip_address"];
    auto attribute_info = attribute.mutable_info();
    attribute.mutable_info()->set_name("ip_address");
    attribute.set_kind(NodeManager::Attribute::STRING);
    attribute.add_s(node_endpoint);


    //Get update endpoint
    //note: This falls out of scope and self destructs at the end of this function,
    //      this is important as we cant destruct our context otherwise

    try{
        auto requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(manager_address_));
        auto reply_future = requester->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                                    ("NodeQuery", message, 3000);
        auto reply = reply_future.get();
        return reply->control_message();

    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error("Environment manager request timed out.");
    }catch(const std::exception& ex){
        std::cerr << "Exception in EnvironmentRequester::NodeQuery" << ex.what() << std::endl;
    }
}


void EnvironmentRequester::Start(){
    auto initial_requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(manager_endpoint_));

    //Register this deployment with the environment manager
    NodeManager::EnvironmentMessage initial_message;

    //map our two enums
    switch(deployment_type_){
        case DeploymentType::RE_MASTER:{
            initial_message.set_type(NodeManager::EnvironmentMessage::ADD_DEPLOYMENT);
            break;
        }
        case DeploymentType::LOGAN:{
            initial_message.set_type(NodeManager::EnvironmentMessage::ADD_LOGAN_CLIENT);
            initial_message.set_update_endpoint(ip_address_);
            break;
        }
        default:{
            std::cerr << "Deployment type invalid in EnvironmentRequester::HeartbeatLoop" << std::endl;
            assert(true);
        }
    }

    initial_message.set_experiment_id(experiment_id_);

    auto initial_reply_future = initial_requester->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>("ExperimentRegistration", initial_message, 3000);

    try{
        auto initial_reply_messsage = initial_reply_future.get();
        if(initial_reply_messsage->type() == NodeManager::EnvironmentMessage::ERROR_RESPONSE){
            std::stringstream str_stream;
            const auto& error_list = initial_reply_messsage->error_messages();
            std::for_each(error_list.begin(), error_list.end(), [&str_stream](const std::string& str){str_stream << "* " << str << "\n";});
            environment_manager_not_found_ = true;
            return;
        }


        manager_update_endpoint_ = initial_reply_messsage->update_endpoint();
    }catch(const zmq::TimeoutException& ex){
        environment_manager_not_found_ = true;
        return;
    }catch(const std::exception& ex){

    }

    update_requester_ = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(manager_update_endpoint_));

    // Start heartbeater
    heartbeater_ = std::unique_ptr<Heartbeater>(new Heartbeater(1000, *update_requester_));
    heartbeater_->AddCallback(std::bind(&EnvironmentRequester::HandleReply, this, std::placeholders::_1));
    heartbeater_->Start();

}

void EnvironmentRequester::Terminate(){
    std::lock_guard<std::mutex> lock(heartbeater_mutex_);
    if(heartbeater_){
        heartbeater_->Terminate();
    }
}

NodeManager::ControlMessage EnvironmentRequester::AddDeployment(const NodeManager::ControlMessage& control_message){
    if(environment_manager_not_found_){
        throw std::runtime_error("Could not add deployment as environment manager was not found.");
    }

    NodeManager::EnvironmentMessage env_message;
    env_message.set_type(NodeManager::EnvironmentMessage::GET_DEPLOYMENT_INFO);
    
    auto current_control_message = env_message.mutable_control_message();
    *current_control_message = control_message;

    try{
        auto reply_future = update_requester_->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                                            ("EnvironmentManagerAddExperiment", env_message, 1000);
        auto reply_msg = reply_future.get();
        if(reply_msg->type() != NodeManager::EnvironmentMessage::SUCCESS){
            std::stringstream str_stream;
            const auto& error_list = reply_msg->error_messages();
            std::for_each(error_list.begin(), error_list.end(), [&str_stream](const std::string& str){str_stream << "* " << str << "\n";});
            throw std::runtime_error("Got non success in EnvironmentRequester::AddDeployment: \n" + str_stream.str());
        }
        return reply_msg->control_message();
    }
    catch(std::exception& ex){
        std::cerr << ex.what() << " in EnvironmentRequester::AddDeployment" << std::endl;
        throw std::runtime_error("Failed to parse message in EnvironmentRequester::AddDeployment");
    }

}

void EnvironmentRequester::RemoveDeployment(){
    NodeManager::EnvironmentMessage message;
    if(deployment_type_ == EnvironmentRequester::DeploymentType::RE_MASTER){
        message.set_type(NodeManager::EnvironmentMessage::REMOVE_DEPLOYMENT);
    }

    if(deployment_type_ == EnvironmentRequester::DeploymentType::LOGAN){
        message.set_type(NodeManager::EnvironmentMessage::REMOVE_LOGAN_CLIENT);
    }

    try{
        auto reply_future = update_requester_->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                                            ("EnvironmentManagerRemoveExperiment", message, 1000);
        auto reply_msg = reply_future.get();
        if(reply_msg->type() == NodeManager::EnvironmentMessage::SUCCESS){
            std::cout << "* Removed from environment manager." << std::endl;
            Terminate();
        }
    }
    catch(std::exception& ex){
        std::cout << ex.what() << " in EnvironmentRequester::RemoveDeployment" << std::endl;
    }
}

NodeManager::EnvironmentMessage EnvironmentRequester::GetLoganInfo(const std::string& node_ip_address){
    if(environment_manager_not_found_){
        throw std::runtime_error("Could not add deployment as environment manager was not found.");
    }

    NodeManager::EnvironmentMessage request_message;

    request_message.set_type(NodeManager::EnvironmentMessage::LOGAN_QUERY);
    request_message.set_experiment_id(experiment_id_);
    request_message.set_update_endpoint(ip_address_);

    NodeManager::EnvironmentMessage reply_message;

    auto response = update_requester_->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                                            ("EnvironmentManagerGetLoganInfo", request_message, 1000);
    try{
        auto response_msg = response.get();
        return NodeManager::EnvironmentMessage(*response_msg);
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error("Get Logan Info request timed out.");
    }catch(const std::exception& ex){

    }
}

void EnvironmentRequester::HandleReply(NodeManager::EnvironmentMessage& message){
    update_callback_(message);
}

