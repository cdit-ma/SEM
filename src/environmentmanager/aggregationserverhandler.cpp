#include "aggregationserverhandler.h"
#include <functional>
#include <zmq/zmqutils.hpp>

AggregationServerHandler::AggregationServerHandler(EnvironmentManager::Environment &environment,
                                                   const std::string &environment_manager_ip_address,
                                                   const std::string &aggregation_server_ip_address,
                                                   std::promise<std::string> port_promise) :
                                                   environment_(environment),
                                                   environment_manager_ip_address_(environment_manager_ip_address),
                                                   aggregation_server_ip_address_(aggregation_server_ip_address){

    auto port = environment_.GetPort(environment_manager_ip_address_);



    // If we want to publish message to agg server
    {
        publisher_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter());
        publisher_->BindPublisherSocket(zmq::TCPify(environment_manager_ip_address_, port));

        publisher_future_ = std::async(std::launch::async, &AggregationServerHandler::PublisherLoop, this);
    }


    // If we want to heartbeat with agg server
    {
        replier_ = std::unique_ptr<zmq::ProtoReplier>(new zmq::ProtoReplier());
        replier_->Bind(zmq::TCPify(environment_manager_ip_address_, port));
        replier_->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                ("AggregationServerHeartbeat", [this](const NodeManager::EnvironmentMessage& message)
                    {return AggregationServerHandler::HandleHeartbeat(message);});

        replier_->Start();
    }

    port_promise.set_value(port);
}

AggregationServerHandler::~AggregationServerHandler(){
    std::lock_guard<std::mutex> lock(update_queue_mutex_);
    end_flag_ = true;
    update_queue_condition_.notify_all();

    if(publisher_future_.valid()){
        publisher_future_.get();
    }
}

void AggregationServerHandler::PublisherLoop() noexcept {
    bool shutdown = false;
    while(!shutdown){

        std::unique_lock<std::mutex> lock(update_queue_mutex_);
        update_queue_condition_.wait(lock, [this]{return end_flag_;});
        if(end_flag_){
            shutdown = true;
        }

        while(!update_message_queue_.empty()){
            publisher_->PushMessage(std::move(update_message_queue_.front()));
            // Remove nullptr from front of queue
            update_message_queue_.pop();
        }
    }
}

void AggregationServerHandler::PushMessage(std::unique_ptr<NodeManager::EnvironmentMessage> message){
    std::lock_guard<std::mutex> lock(update_queue_mutex_);
    update_message_queue_.push(std::move(message));
    update_queue_condition_.notify_all();
}

void AggregationServerHandler::HeartbeatLoop() noexcept {

}

std::unique_ptr<NodeManager::EnvironmentMessage> AggregationServerHandler::HandleHeartbeat(const NodeManager::EnvironmentMessage& message) {

    return std::unique_ptr<NodeManager::EnvironmentMessage>();
}

bool AggregationServerHandler::IsRemovable() {
    throw std::runtime_error("AggregationServerHandler::IsRemovable NOT IMPLEMENTED");
    //return false;
}


