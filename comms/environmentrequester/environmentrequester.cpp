#include "environmentrequester.h"

#include <proto/controlmessage/controlmessage.pb.h>
#include <zmq/protorequester/protorequester.hpp>
#include <sstream>

std::unique_ptr<NodeManager::NodeManagerRegistrationReply>
EnvironmentRequest::TryRegisterNodeManager(const std::string& environment_manager_endpoint, const std::string& experiment_name, const std::string& node_ip_address) {
    NodeManager::NodeManagerRegistrationRequest request;

    request.mutable_id()->set_experiment_name(experiment_name);
    request.mutable_id()->set_ip_address(node_ip_address);


    // Retry three times
    int retry_count = 0;
    while(retry_count > 3){
        try{
            auto requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(manager_address_));
            auto reply_future = requester->SendRequest<NodeManager::NodeManagerRegistrationRequest, NodeManager::NodeManagerRegistrationReply>
                    ("NodeManagerRegistration", request, 1000);
            return reply_future.get();
        }catch(const zmq::TimeoutException& ex){
            retry_count++;
        }catch(const std::exception& ex){
            std::cerr << "Exception in EnvironmentRequester::NodeQuery" << ex.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    throw zmq::TimeoutException("TryRegisterNodeManager failed after three attempts.");
}

std::unique_ptr<NodeManager::LoganRegistrationReply>
EnvironmentRequest::TryRegisterLoganServer(const std::string& environment_manager_endpoint, const std::string& experiment_name, const std::string& node_ip_address) {
    NodeManager::LoganRegistrationRequest request;

    request.mutable_id()->set_experiment_name(experiment_name);
    request.mutable_id()->set_ip_address(node_ip_address);

    int retry_count = 0;

    while(retry_count > 3){
        try{
            auto requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(manager_address_));
            auto reply_future = requester->SendRequest<NodeManager::LoganRegistrationRequest, NodeManager::LoganRegistrationReply>
                    ("LoganRegistration", request, 1000);
            return reply_future.get();
        }catch(const zmq::TimeoutException& ex){
            retry_count++;
        }catch(const std::exception& ex){
            std::cerr << "Exception in EnvironmentRequester::NodeQuery" << ex.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    throw zmq::TimeoutException("TryRegisterLoganServer failed after three attempts.");
}

EnvironmentRequest::NodeManagerHeartbeatRequester::NodeManagerHeartbeatRequester(
        const std::string& experiment_name,
        const std::string& node_ip_address,
        const std::string& heartbeat_endpoint,
        std::function<void (NodeManager::EnvironmentMessage &)> configure_function) :
        experiment_name_(experiment_name),
        node_ip_address_(node_ip_address){

    requester_ = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(heartbeat_endpoint));
    heartbeater_ = std::unique_ptr<Heartbeater>(new Heartbeater(1000, *requester_));
    heartbeater_->AddCallback(configure_function);
    heartbeater_->Start();
}

EnvironmentRequest::NodeManagerHeartbeatRequester::~NodeManagerHeartbeatRequester() {
    Terminate();
}

void EnvironmentRequest::NodeManagerHeartbeatRequester::Terminate() {
    std::lock_guard<std::mutex> lock(heartbeater_mutex_);
    if(heartbeater_){
        heartbeater_->Terminate();
        heartbeater_.reset();
    }
}

void EnvironmentRequest::NodeManagerHeartbeatRequester::RemoveDeployment() {
    NodeManagerDeregisterationRequest request;
    request.mutable_id()->set_experiment_name(experiment_name_);
    request.mutable_id()->set_ip_address(node_ip_address_);

    auto reply_future = requester_->SendRequest<NodeManager::NodeManagerDeregistrationRequest, NodeManager::NodeManagerDeregistrationReply>("NodeManagerDeregisteration", request, 1000);
    reply_future.get();
}

std::unique_ptr<NodeManager::EnvironmentMessage> EnvironmentRequest::NodeManagerHeartbeatRequester::GetExperimentInfo() {
    NodeManager::EnvironmentMessage request;
    request.set_type(NodeManager::EnvironmentMessage::GET_EXPERIMENT_INFO);
    auto reply_future = requester_->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>("GetExperimentInfo", request, 1000);
    return reply_future.get();
}