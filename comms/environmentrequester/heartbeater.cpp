#include "heartbeater.h"

Heartbeater::Heartbeater(const int heartbeat_period, zmq::ProtoRequester& requester) : 
heartbeat_period_(heartbeat_period),
requester_(requester)
{}

Heartbeater::~Heartbeater(){
    std::cout << "~Heartbeater" << std::endl;
    Terminate();
}

void Heartbeater::HeartbeatLoop(){
    int retry_count = 0;
    // Start heartbeat loop
    while(retry_count < 5){

        {
            std::unique_lock<std::mutex> lock(heartbeat_lock_);

            // Wait for a message on the queue OR our heartbeat period to time out.
            heartbeat_cv_.wait_for(lock, std::chrono::milliseconds(heartbeat_period_));

            if(end_flag_){
    //std::cout << "TERMINATEasdf" << std::endl;

                break;
            }
        }

        NodeManager::EnvironmentMessage message;
        message.set_type(NodeManager::EnvironmentMessage::HEARTBEAT);

        try{
            auto reply_future = requester_.SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                                            ("EnvironmentManagerHeartbeat", message, heartbeat_period_);
            auto reply_message = reply_future.get();
            // Reset our retry count
            retry_count = 0;
            HandleReply(*reply_message);
        }catch(const zmq::TimeoutException& ex){
            retry_count ++;
            std::cerr << "Heartbeat timed out: Retry # " << retry_count << std::endl;
        }catch(const std::exception& ex){
            retry_count ++;
            std::cerr << "EnvironmentRequester::HeartbeatLoop handle reply " << ex.what() << std::endl;
        }
    }

    if(retry_count >= 5){
        std::cerr << "Heartbeater::HeartbeatLoop Timed out" << std::endl;
    }
}

void Heartbeater::Start(){
    heartbeat_future_ = std::async(std::launch::async, &Heartbeater::HeartbeatLoop, this);
    std::cout << "started" << std::endl;
}

void Heartbeater::Terminate(){
    std::lock_guard<std::mutex> lock(heartbeat_lock_);
    end_flag_ = true;
    heartbeat_cv_.notify_one();
    heartbeat_future_.get();
}

void Heartbeater::AddCallback(std::function<void (NodeManager::EnvironmentMessage& environment_message)> callback_func){
    callback_func_ = callback_func;
}

void Heartbeater::HandleReply(NodeManager::EnvironmentMessage& environment_message){
    switch(environment_message.type()){
        case NodeManager::EnvironmentMessage::HEARTBEAT_ACK:{
            // no-op
            return;
        }
        default:{
            if(callback_func_){
                callback_func_(environment_message);
            }else{
                throw std::runtime_error("Update callback not set");
            }
        }
    }
}