#include "heartbeater.h"

Heartbeater::Heartbeater(const int heartbeat_period, zmq::ProtoRequester& requester) : requester_(requester){}


void Heartbeater::HeartbeatLoop(){
    int retry_count = 0;
    // Start heartbeat loop
    while(retry_count < 5){

        {
            std::unique_lock<std::mutex> lock(heartbeat_lock_);

            // Wait for a message on the queue OR our heartbeat period to time out.
            heartbeat_cv_.wait_for(lock, std::chrono::milliseconds(HEARTBEAT_PERIOD));

            if(end_flag_){
                break;
            }
        }

        NodeManager::EnvironmentMessage message;
        message.set_type(NodeManager::EnvironmentMessage::HEARTBEAT);

        try{
            auto reply_future = requester_.SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                                            ("EnvironmentManagerHeartbeat", message, HEARTBEAT_TIMEOUT);
            auto reply_message = reply_future.get();
            // Reset our retry count
            retry_count = 0;
            HandleReply(*reply_message);
        }catch(const zmq::TimeoutException& ex){
            retry_count ++;
            std::cerr << "Heartbeat timed out: Retry # " << retry_count << std::endl;
        }catch(const std::exception& ex){
            retry_count ++;
            // TODO: call registered exception handling callback??
            std::cerr << "EnvironmentRequester::HeartbeatLoop handle reply " << ex.what() << std::endl;
        }
    }

    if(retry_count >= 5){
        std::cerr << "EnvironmentRequester::HeartbeatLoop Timed out" << std::endl;
    }
}

void Heartbeater::Start(){
    heartbeat_future_ = std::async(std::launch::async, &EnvironmentRequester::HeartbeatLoop, this);
}

void Heartbeater::Terminate(){

}