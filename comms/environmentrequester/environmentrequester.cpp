#include "environmentrequester.h"

#include <proto/controlmessage/controlmessage.pb.h>
#include <zmq/protorequester/protorequester.hpp>
#include <sstream>

std::unique_ptr<NodeManager::NodeManagerRegistrationReply>
EnvironmentRequest::TryRegisterNodeManager(const std::string& environment_manager_endpoint, const std::string& experiment_name, const std::string& node_ip_address){
    const int retry_ms = 1000;
    NodeManager::NodeManagerRegistrationRequest request;

    request.mutable_id()->set_experiment_name(experiment_name);
    request.mutable_id()->set_ip_address(node_ip_address);


    auto requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(environment_manager_endpoint));
    
    // Retry three times
    int retry_count = 0;
    while(retry_count < 3){
        try{
            auto reply_future = requester->SendRequest<NodeManager::NodeManagerRegistrationRequest, NodeManager::NodeManagerRegistrationReply>
                    ("NodeManagerRegistration", request, retry_ms);
            return reply_future.get();
        }catch(const zmq::TimeoutException& ex){
            retry_count++;
        }catch(const zmq::RMIException& ex){
            std::cerr << "* RMI Exception: " << ex.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_ms));
            retry_count++;
        }catch(const std::exception& ex){
            std::cerr << "Exception in EnvironmentRequester::TryRegisterNodeManager" << ex.what() << std::endl;
            throw;
        }
    }
    throw zmq::TimeoutException("TryRegisterNodeManager failed after three attempts.");
}

std::unique_ptr<NodeManager::LoganRegistrationReply>
EnvironmentRequest::TryRegisterLoganServer(const std::string& environment_manager_endpoint, const std::string& experiment_name, const std::string& node_ip_address) {
    const int retry_ms = 1000;
    NodeManager::LoganRegistrationRequest request;

    request.mutable_id()->set_experiment_name(experiment_name);
    request.mutable_id()->set_ip_address(node_ip_address);

    
    auto requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(environment_manager_endpoint));

    int retry_count = 0;
    while(retry_count < 3){
        try{
            auto reply_future = requester->SendRequest<NodeManager::LoganRegistrationRequest, NodeManager::LoganRegistrationReply>
                    ("LoganRegistration", request, retry_ms);
            return reply_future.get();
        }catch(const zmq::TimeoutException& ex){
            retry_count++;
        }catch(const zmq::RMIException& ex){
            std::cerr << "* RMI Exception: " << ex.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_ms));
            retry_count++;
        }catch(const std::exception& ex){
            std::cerr << "Exception in EnvironmentRequester::TryRegisterLoganServer" << ex.what() << std::endl;
            throw;
        }
    }
    throw zmq::TimeoutException("TryRegisterLoganServer failed after three attempts.");
}

EnvironmentRequest::HeartbeatRequester::HeartbeatRequester(
        const std::string& heartbeat_endpoint,
        std::function<void (NodeManager::EnvironmentMessage &)> configure_function){

    requester_ = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(heartbeat_endpoint));
    heartbeater_ = std::unique_ptr<Heartbeater>(new Heartbeater(1000, *requester_));
    heartbeater_->AddCallback(configure_function);
    heartbeater_->Start();
}

EnvironmentRequest::HeartbeatRequester::~HeartbeatRequester() {
    Terminate();
}

void EnvironmentRequest::HeartbeatRequester::Terminate() {
    std::lock_guard<std::mutex> lock(heartbeater_mutex_);
    if(heartbeater_){
        heartbeater_->Terminate();
        heartbeater_.reset();
    }
}

void EnvironmentRequest::HeartbeatRequester::SetTimeoutCallback(std::function<void (void)> timeout_func){
    std::lock_guard<std::mutex> lock(heartbeater_mutex_);
    if(heartbeater_){
        heartbeater_->SetTimeoutCallback(timeout_func);
    }
}

void EnvironmentRequest::HeartbeatRequester::RemoveDeployment() {
    NodeManager::NodeManagerDeregistrationRequest request;
    auto reply_future = requester_->SendRequest<NodeManager::NodeManagerDeregistrationRequest, NodeManager::NodeManagerDeregistrationReply>("NodeManagerDeregisteration", request, 1000);
    reply_future.get();
}

std::unique_ptr<NodeManager::EnvironmentMessage> EnvironmentRequest::HeartbeatRequester::GetExperimentInfo() {
    NodeManager::EnvironmentMessage request;
    request.set_type(NodeManager::EnvironmentMessage::GET_EXPERIMENT_INFO);
    auto reply_future = requester_->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>("GetExperimentInfo", request, 1000);
    return reply_future.get();
}
