#include "environmentrequester.h"

EnvironmentRequester::EnvironmentRequester(const std::string& manager_address, 
                                            const std::string& deployment_id, 
                                            const std::string& deployment_info){
    manager_address_ = manager_address;
    deployment_id_ = deployment_id;
    deployment_info_ = deployment_info;
}

void EnvironmentRequester::Init(){
    try{
        context_ = new zmq::context_t(1);
        zmq::socket_t sub(*context_, ZMQ_SUB);
        sub.connect(manager_address_);
        sub.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        zmq::message_t message;
        sub.recv(&message);
        manager_endpoint_ = std::string(static_cast<const char*>(message.data()), message.size());
        sub.close();
    }
    catch(std::exception& ex){
        std::cout << ex.what() << " in EnvironmentRequester::Init" << std::endl;
    }
}

void EnvironmentRequester::Start(){
    heartbeat_thread_ = new std::thread(&EnvironmentRequester::HeartbeatLoop, this);
}

uint64_t EnvironmentRequester::Tick(){
    std::unique_lock<std::mutex>(clock_mutex_);
    clock_++;
    return clock_;
}

uint64_t EnvironmentRequester::SetClock(uint64_t incoming_clock){
    std::unique_lock<std::mutex>(clock_mutex_);
    clock_ = std::max(incoming_clock, clock_) + 1;
    return clock_;
}

uint64_t EnvironmentRequester::GetClock(){
    return clock_;
}

void EnvironmentRequester::HeartbeatLoop(){
    zmq::socket_t* initial_request_socket = new zmq::socket_t(*context_, ZMQ_REQ);
    initial_request_socket->connect(manager_endpoint_);

    //Register this deployment with the environment manager
    ZMQSendTwoPartRequest(initial_request_socket, "DEPLOYMENT", deployment_id_);
    auto reply = ZMQReceiveTwoPartReply(initial_request_socket);

    //Get update socket address as reply
    if(reply.reply_type_.compare("SUCCESS") == 0){
        manager_update_endpoint_ = reply.reply_data_;
    }
    else{
        //TODO: Handle this error
        std::cout << "HANDLE THIS ERROR! 1" << std::endl;
    }

    //Connect to our update socket
    try{
        update_socket_ = new zmq::socket_t(*context_, ZMQ_REQ);
        update_socket_->connect(manager_update_endpoint_);
    }
    catch(std::exception& ex){
        std::cout << ex.what() << " in EnvironmentRequester::Start" << std::endl;
    }

    //Start heartbeat loop
    while(true){
        //If our request queue is empty, wait till time for next heartbeat and send unless we get a wake up in that time
        if(request_queue_.empty()){
            std::unique_lock<std::mutex> lock(request_queue_lock_);
            auto trigger = request_queue_cv_.wait_for(lock, std::chrono::duration<int>(heartbeat_period_));

            if(end_flag_){
                break;
            }

            if(trigger == std::cv_status::timeout){
                ZMQSendTwoPartRequest(update_socket_, "HEARTBEAT", "");
                auto reply = ZMQReceiveTwoPartReply(update_socket_);
            }
            else if(trigger == std::cv_status::no_timeout){
                auto request = request_queue_.front();
                request_queue_.pop();
                SendRequest(request);
            }
        }

        //If we have something in our request queue, send it immediately
        else{
            std::unique_lock<std::mutex> lock(request_queue_lock_);
            auto request = request_queue_.front();
            request_queue_.pop();
            SendRequest(request);
        }
    }
}

void EnvironmentRequester::End(){
    end_flag_ = true;
    heartbeat_thread_->join();
}

int EnvironmentRequester::GetPort(const std::string& component_id, const std::string& component_info){
    int port = 0;
    auto port_response = QueueRequest("ASSIGNMENT_REQUEST", component_id);

    try{
        port = std::stoi(port_response.get());
    }
    catch(std::exception& ex){
        std::cout << ex.what() << " in EnvironmentRequester::GetPort" << std::endl;
    }
    return port;
}

std::future<std::string> EnvironmentRequester::QueueRequest(const std::string& request_type, const std::string& request){

    std::promise<std::string>* request_promise = new std::promise<std::string>();
    std::future<std::string> request_future = request_promise->get_future();
    Request request_struct = {request_type, request, request_promise};

    std::unique_lock<std::mutex> lock(request_queue_lock_);
    request_queue_.push(request_struct);
    lock.unlock();
    request_queue_cv_.notify_one();
    return request_future;
}

void EnvironmentRequester::SendRequest(EnvironmentRequester::Request request){
    ZMQSendTwoPartRequest(update_socket_, request.request_type_, request.request_data_);
    auto reply = ZMQReceiveTwoPartReply(update_socket_);

    //Handle response
    if(reply.reply_type_.compare("SUCCESS") == 0){
        request.response_->set_value(reply.reply_data_);
    }
    else{
        //TODO: Handle this failure/other response
    }
}

void EnvironmentRequester::ZMQSendTwoPartRequest(zmq::socket_t* socket, const std::string& request_type, 
                                                                        const std::string& request){
    std::string lamport_string = std::to_string(Tick());
    zmq::message_t lamport_time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t request_type_msg(request_type.begin(), request_type.end());
    zmq::message_t request_msg(request.begin(), request.end());


    try{
        socket->send(request_type_msg, ZMQ_SNDMORE);
        socket->send(lamport_time_msg, ZMQ_SNDMORE);
        socket->send(request_msg);
    }
    catch(std::exception error){
        //TODO: Handle this
        std::cout << error.what() << "in EnvironmentRequester::ZMQSendTwoPartRequest" << std::endl;
    }
}

EnvironmentRequester::Reply EnvironmentRequester::ZMQReceiveTwoPartReply(zmq::socket_t* socket){
    zmq::message_t request_type_msg;
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;
    try{
        socket->recv(&request_type_msg);
        socket->recv(&lamport_time_msg);
        socket->recv(&request_contents_msg);
    }
    catch(zmq::error_t error){
        //TODO: Handle this
        std::cout << error.what() << " in EnvironmentRequester::ZMQReceiveTwoPartReply" << std::endl;
    }
    std::string type(static_cast<const char*>(request_type_msg.data()), request_type_msg.size());
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());
    long lamport_time = SetClock(std::stoull(incoming_time));

    return Reply{type, contents};
}
