#include "heartbeater.h"

Heartbeater::Heartbeater(const int heartbeat_period, zmq::ProtoRequester& requester) : 
heartbeat_period_(heartbeat_period),
requester_(requester)
{}

Heartbeater::~Heartbeater(){
    Terminate();
}

void Heartbeater::HeartbeatLoop(){
    int retry_count = 0;
    // Start heartbeat loop
    bool shutdown = false;
    while(retry_count < 5 && !shutdown){
        {
            std::unique_lock<std::mutex> lock(heartbeat_lock_);
            // Wait for a message on the queue OR our heartbeat period to time out.
            heartbeat_cv_.wait_for(lock, std::chrono::milliseconds(heartbeat_period_), [this]{return end_flag_;});
            if(end_flag_){
                shutdown = true;
            }
        }

        NodeManager::EnvironmentMessage message;
        message.set_type(shutdown ? NodeManager::EnvironmentMessage::END_HEARTBEAT : NodeManager::EnvironmentMessage::HEARTBEAT);

        try{
            auto reply_future = requester_.SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                                            ("NodeManagerHeartbeat", message, heartbeat_period_);
            auto reply_message = reply_future.get();
            // Reset our retry count
            retry_count = 0;

            HandleReply(*reply_message);
        }catch(const zmq::TimeoutException& ex){
            retry_count ++;
            if(!shutdown){
                //Not Expecting a response from the END_HEARTBEAT
                std::cerr << "Heartbeat timed out: Retry # " << retry_count << std::endl;
            }
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
}

void Heartbeater::Terminate(){
    {
        std::lock_guard<std::mutex> lock(heartbeat_lock_);

        if(!end_flag_){
            end_flag_ = true;
            heartbeat_cv_.notify_all();
        }
    }
    if(heartbeat_future_.valid()){
        heartbeat_future_.get();
    }
}

void Heartbeater::AddCallback(std::function<void (NodeManager::EnvironmentMessage& environment_message)> callback_func){
    callback_func_ = std::move(callback_func);
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