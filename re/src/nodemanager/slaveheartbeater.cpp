#include "slaveheartbeater.h"
#include <proto/controlmessage/controlmessage.pb.h>


SlaveHeartbeater::SlaveHeartbeater(zmq::ProtoRequester& requester, const std::string& ip_address, const std::string& container_id):
    requester_(requester),
    ip_address_(ip_address),
    container_id_(container_id),
    heartbeat_period_(1000)
    {

    heartbeat_future_ = std::async(std::launch::async, &SlaveHeartbeater::HeartbeatLoop, this);
};

SlaveHeartbeater::~SlaveHeartbeater(){
    Terminate();

    try{
        if(heartbeat_future_.valid()){
            heartbeat_future_.get();
        }
    }catch(const std::exception& ex){
        std::cerr << ex.what() << std::endl;
    }
}

void SlaveHeartbeater::SetTimeoutCallback(std::function<void (void)> timeout_callback){
    timeout_callback_ = std::move(timeout_callback);
}

void SlaveHeartbeater::Terminate(){
    {
        std::lock_guard<std::mutex> lock(heartbeat_lock_);
        end_flag_ = true;
    }
    heartbeat_cv_.notify_all();
}




void SlaveHeartbeater::HeartbeatLoop(){
    int retry_count = 0;
    // Start heartbeat loop
    bool shutdown = false;

    while(retry_count < 5 && !shutdown){
        {
            std::unique_lock<std::mutex> lock(heartbeat_lock_);
            // Wait for a message on the queue OR our heartbeat period to time out.
            heartbeat_cv_.wait_for(lock, heartbeat_period_, [this]{return end_flag_;});
            if(end_flag_){
                shutdown = true;
            }
        }

        NodeManager::SlaveHeartbeatRequest heartbeat;
        heartbeat.set_type(shutdown ? NodeManager::SlaveHeartbeatRequest::END_HEARTBEAT : NodeManager::SlaveHeartbeatRequest::HEARBEAT);
        heartbeat.mutable_id()->set_ip_address(ip_address_);
        heartbeat.mutable_id()->set_container_id(container_id_);
        try{
            //std::cerr << "Slave Heartbeating: " << ip_address_ << " " << container_id_ << std::endl;
            auto reply_future = requester_.SendRequest<NodeManager::SlaveHeartbeatRequest, NodeManager::SlaveHeartbeatReply>
                                                            ("SlaveHeartbeat", heartbeat, heartbeat_period_.count());
            auto reply = reply_future.get();
            //reset the retry_count
            retry_count = 0;
        }catch(const zmq::TimeoutException& ex){
            retry_count ++;
        }catch(const std::exception& ex){
            retry_count ++;
            std::cerr << "* SlaveHeartbeater::HeartbeatLoop: Threw exception '" << ex.what() << "'" << std::endl;
        }
    }

    if(retry_count >= 5){
        std::cerr << "* SlaveHeartbeater::HeartbeatLoop Timed out" << std::endl;
        if(timeout_callback_){
            timeout_callback_();
        }
    }
}
