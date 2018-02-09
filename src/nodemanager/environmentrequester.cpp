#include "environmentrequester.h"
#include "../environmentmanager/environmentmessage/environmentmessage.pb.h"

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
        std::cerr << ex.what() << " in EnvironmentRequester::Init" << std::endl;
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
    EnvironmentManager::EnvironmentMessage initial_message;
    initial_message.set_type(EnvironmentManager::EnvironmentMessage::ADD_DEPLOYMENT);
    auto deployment = initial_message.add_deployments();
    deployment->set_id(deployment_id_);
    auto manager_endpoint = deployment->add_endpoints();
    manager_endpoint->set_type(EnvironmentManager::Endpoint::MANAGEMENT);
    ZMQSendRequest(initial_request_socket, initial_message.SerializeAsString());
    auto reply = ZMQReceiveReply(initial_request_socket);

    //Get update socket address as reply
    if(!reply.empty()){
        EnvironmentManager::EnvironmentMessage initial_reply;
        initial_reply.ParseFromString(reply);
        manager_update_endpoint_ = initial_reply.update_endpoint();
    }
    else{
        //TODO: Handle this error
        std::cerr << "HANDLE THIS ERROR! 1" << std::endl;
    }

    //Connect to our update socket
    try{
        update_socket_ = new zmq::socket_t(*context_, ZMQ_REQ);
        update_socket_->connect(manager_update_endpoint_);
    }
    catch(std::exception& ex){
        std::cerr << ex.what() << " in EnvironmentRequester::Start" << std::endl;
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
            EnvironmentManager::EnvironmentMessage message;
            message.set_type(EnvironmentManager::EnvironmentMessage::HEARTBEAT);
            if(trigger == std::cv_status::timeout){
                std::string output;
                message.SerializeToString(&output);
                ZMQSendRequest(update_socket_, output);
                auto reply = ZMQReceiveReply(update_socket_);
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

int EnvironmentRequester::GetComponentPort(const std::string& component_id, const std::string& component_info){
    int port = 0;

    EnvironmentManager::EnvironmentMessage message;
    message.set_type(EnvironmentManager::EnvironmentMessage::ADD_COMPONENT);
    auto component = message.add_components();
    component->set_id(component_id);
    auto endpoint = component->add_endpoints();
    endpoint->set_id("asdf");
    endpoint->set_type(EnvironmentManager::Endpoint::PUBLIC);

    auto response = QueueRequest(message.SerializeAsString());

    try{
        EnvironmentManager::EnvironmentMessage response_message;
        response_message.ParseFromString(response.get());
        port = std::stoi(response_message.components(0).endpoints(0).port());
    }
    catch(std::exception& ex){
        std::cerr << ex.what() << " in EnvironmentRequester::GetPort" << std::endl;
    }
    return port;
}

std::future<std::string> EnvironmentRequester::QueueRequest(const std::string& request){

    std::promise<std::string>* request_promise = new std::promise<std::string>();
    std::future<std::string> request_future = request_promise->get_future();
    Request request_struct = {request, request_promise};

    std::unique_lock<std::mutex> lock(request_queue_lock_);
    request_queue_.push(request_struct);
    lock.unlock();
    request_queue_cv_.notify_one();
    return request_future;
}

void EnvironmentRequester::SendRequest(EnvironmentRequester::Request request){
    ZMQSendRequest(update_socket_, request.request_data_);
    auto reply = ZMQReceiveReply(update_socket_);

    //Handle response
    request.response_->set_value(reply);
}

void EnvironmentRequester::ZMQSendRequest(zmq::socket_t* socket, const std::string& request){
    std::string lamport_string = std::to_string(Tick());
    zmq::message_t lamport_time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t request_msg(request.begin(), request.end());

    try{
        socket->send(lamport_time_msg, ZMQ_SNDMORE);
        socket->send(request_msg);
    }
    catch(std::exception error){
        //TODO: Handle this
        std::cerr << error.what() << "in EnvironmentRequester::ZMQSendTwoPartRequest" << std::endl;
    }
}

std::string EnvironmentRequester::ZMQReceiveReply(zmq::socket_t* socket){
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;
    try{
        socket->recv(&lamport_time_msg);
        socket->recv(&request_contents_msg);
    }
    catch(zmq::error_t error){
        //TODO: Handle this
        std::cerr << error.what() << " in EnvironmentRequester::ZMQReceiveTwoPartReply" << std::endl;
    }
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());
    long lamport_time = SetClock(std::stoull(incoming_time));

    return contents;
}
