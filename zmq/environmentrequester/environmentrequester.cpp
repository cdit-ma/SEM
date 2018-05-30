#include "environmentrequester.h"
#include <controlmessage.pb.h>
EnvironmentRequester::EnvironmentRequester(const std::string& manager_address, 
                                            const std::string& experiment_id,
                                            EnvironmentRequester::DeploymentType deployment_type){
    manager_address_ = manager_address;
    experiment_id_ = experiment_id;
    deployment_type_ = deployment_type;
}

void EnvironmentRequester::Init(){
    try{
        context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
        zmq::socket_t sub(*context_, ZMQ_SUB);
        sub.connect(manager_address_);
        sub.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        zmq::message_t message;

        std::vector<zmq::pollitem_t> poll_items = {{sub, 0, ZMQ_POLLIN, 0}};
        int events = zmq::poll(poll_items, REQUEST_TIMEOUT);

        if(events >= 1){
            sub.recv(&message);
            manager_endpoint_ = std::string(static_cast<const char*>(message.data()), message.size());
            sub.close();
        }
        else{
            std::cerr << "Wait for environment broadcast message timed out in EnvironmentRequester: " << experiment_id_ << std::endl;
            assert(false);
        }

    }
    catch(std::exception& ex){
        std::cerr << ex.what() << " in EnvironmentRequester::Init" << std::endl;
    }
}

void EnvironmentRequester::Init(const std::string& manager_endpoint){
    context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
    manager_endpoint_ = manager_endpoint;
}

NodeManager::ControlMessage EnvironmentRequester::NodeQuery(const std::string& node_endpoint){
    //Construct query message

    NodeManager::EnvironmentMessage message;
    message.set_experiment_id(experiment_id_);
    message.set_type(NodeManager::EnvironmentMessage::NODE_QUERY);

    auto control_message = message.mutable_control_message();

    auto node = control_message->add_nodes();
    auto info = node->mutable_info();
    auto attribute = node->add_attributes();
    auto attribute_info = attribute->mutable_info();
    attribute_info->set_name("ip_address");
    attribute->set_kind(NodeManager::Attribute::STRING);
    attribute->add_s(node_endpoint);


    //Get update endpoint
    //note: This falls out of scope and self destructs at the end of this function,
    //      this is important as we cant destruct our context otherwise
    zmq::socket_t initial_request_socket(*context_, ZMQ_REQ);

    initial_request_socket.connect(manager_address_);

    ZMQSendRequest(initial_request_socket, message.SerializeAsString());
    auto reply = ZMQReceiveReply(initial_request_socket);

    if(reply.empty()){
        throw std::runtime_error("Environment manager request timed out.");
    }

    NodeManager::EnvironmentMessage reply_message;

    reply_message.ParseFromString(reply);

    return reply_message.control_message();
}

void EnvironmentRequester::Start(){
    try{
        heartbeat_thread_ = std::unique_ptr<std::thread>(new std::thread(&EnvironmentRequester::HeartbeatLoop, this));
    }
    catch(std::exception& ex){
        std::cout << ex.what() << " in EnvironmentRequester::Start" << std::endl;
    }
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
    std::unique_ptr<zmq::socket_t> initial_request_socket;
    if(context_){
        initial_request_socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(*context_, ZMQ_REQ));
    }
    else{
        std::cerr << "Context in EnvironmentRequester::HeartbeatLoop not initialised." << std::endl;
    }

    try{
        initial_request_socket->connect(manager_endpoint_);
    }
    catch(std::exception& ex){
        std::cerr << ex.what() << " in EnvironmentRequester::HeartbeatLoop" << std::endl;
    }

    //Register this deployment with the environment manager
    NodeManager::EnvironmentMessage initial_message;

    //map our two enums
    switch(deployment_type_){
        case DeploymentType::RE_MASTER:{
            initial_message.set_type(NodeManager::EnvironmentMessage::ADD_DEPLOYMENT);
            break;
        }

        case DeploymentType::LOGAN_CLIENT:{
            initial_message.set_type(NodeManager::EnvironmentMessage::ADD_LOGAN_CLIENT);
            break;
        }

        default:{
            throw std::invalid_argument("Unknown deployment type in EnvironmentRequester::HeartbeatLoop!");
        }
    }


    initial_message.set_experiment_id(experiment_id_);

    ZMQSendRequest(*initial_request_socket, initial_message.SerializeAsString());
    auto reply = ZMQReceiveReply(*initial_request_socket);

    //Get update socket address as reply
    if(!reply.empty()){
        NodeManager::EnvironmentMessage initial_reply;
        initial_reply.ParseFromString(reply);
        manager_update_endpoint_ = initial_reply.update_endpoint();
    }
    else{
        std::cerr << "Environment manager not found at: " << manager_endpoint_ << std::endl;
        std::cerr << "Terminating." << std::endl;
        std::unique_lock<std::mutex> lock(request_queue_lock_);
        if(!request_queue_.empty()){
            while(!request_queue_.empty()){
                auto request = request_queue_.front();
                request_queue_.pop();
                request.response_->set_value("");
            }
        }
        environment_manager_not_found_ = true;
        return;
    }

    //Connect to our update socket
    try{
        update_socket_ = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(*context_, ZMQ_REQ));
        update_socket_->connect(manager_update_endpoint_);
    }
    catch(std::exception& ex){
        std::cerr << ex.what() << " in EnvironmentRequester::HeartbeatLoop" << std::endl;
    }

    //Start heartbeat loop
    while(true){
        //If our request queue is empty, wait till time for next heartbeat and send unless we get a wake up in that time
        if(request_queue_.empty()){
            std::unique_lock<std::mutex> lock(request_queue_lock_);
            auto trigger = request_queue_cv_.wait_for(lock, std::chrono::milliseconds(HEARTBEAT_PERIOD));

            if(end_flag_){
                break;
            }
            NodeManager::EnvironmentMessage message;
            message.set_type(NodeManager::EnvironmentMessage::HEARTBEAT);
            //CV timed out, send heartbeat
            if(trigger == std::cv_status::timeout){
                std::string output;
                message.SerializeToString(&output);
                ZMQSendRequest(*update_socket_, output);
                auto reply = ZMQReceiveReply(*update_socket_);
                if(reply.empty()){
                    //TODO: Run shutdown callback from here!
                    std::cerr << "Heartbeat response from environment manager timed out!" << std::endl;
                }
                NodeManager::EnvironmentMessage reply_message;
                reply_message.ParseFromString(reply);

                try{
                    HandleReply(reply_message);
                }
                catch(const std::exception& ex){
                    //todo: call registered exception handling callback??
                    std::cerr << "EnvironmentRequester::HeartbeatLoop handle reply " << ex.what() << std::endl;
                }

            }
            //CV got wakeup, take from request queue
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
    request_queue_cv_.notify_one();
    heartbeat_thread_->join();
}

NodeManager::ControlMessage EnvironmentRequester::AddDeployment(NodeManager::ControlMessage& control_message){
    if(environment_manager_not_found_){
        throw std::runtime_error("Could not add deployment as environment manager was not found.");
    }
    NodeManager::EnvironmentMessage env_message;
    auto current_control_message = env_message.mutable_control_message();

    *current_control_message = control_message;

    env_message.set_type(NodeManager::EnvironmentMessage::GET_DEPLOYMENT_INFO);

    NodeManager::EnvironmentMessage message;
    try{
        auto response = QueueRequest(env_message.SerializeAsString());
        auto response_msg = response.get();
        if(response_msg.empty()){
            throw std::runtime_error("Got empty response message in EnvironmentRequester::AddDeployment");
        }
        message.ParseFromString(response_msg);
    }
    catch(std::exception& ex){
        std::cerr << ex.what() << " in EnvironmentRequester::AddDeployment" << std::endl;
        throw std::runtime_error("Failed to parse message in EnvironmentRequester::AddDeployment");
    }

    return message.control_message();
}

void EnvironmentRequester::RemoveDeployment(){
    NodeManager::EnvironmentMessage message;
    if(deployment_type_ == EnvironmentRequester::DeploymentType::RE_MASTER){
        message.set_type(NodeManager::EnvironmentMessage::REMOVE_DEPLOYMENT);
    }

    if(deployment_type_ == EnvironmentRequester::DeploymentType::LOGAN_CLIENT){
        message.set_type(NodeManager::EnvironmentMessage::REMOVE_LOGAN_CLIENT);
    }

    auto response = QueueRequest(message.SerializeAsString());

    try{
        NodeManager::EnvironmentMessage response_msg;
        response_msg.ParseFromString(response.get());
        if(response_msg.type() == NodeManager::EnvironmentMessage::SUCCESS){
            std::cout << "Removed from environment manager." << std::endl;
        }
    }
    catch(std::exception& ex){
        std::cout << ex.what() << " in EnvironmentRequester::RemoveDeployment" << std::endl;
    }
}

std::string EnvironmentRequester::GetLoganClientInfo(const std::string& node_ip_address){
    if(environment_manager_not_found_){
        throw std::runtime_error("Could not add deployment as environment manager was not found.");
    }

    NodeManager::EnvironmentMessage request_message;

    request_message.set_type(NodeManager::EnvironmentMessage::LOGAN_QUERY);
    request_message.set_experiment_id(experiment_id_);

    auto logger = request_message.mutable_logger();

    logger->set_publisher_address(node_ip_address);

    NodeManager::EnvironmentMessage reply_message;

    try{
        auto response = QueueRequest(request_message.SerializeAsString());
        auto response_msg = response.get();
        if(response_msg.empty()){
            throw std::runtime_error("Got empty response message in EnvironmentRequester::AddDeployment");
        }
        reply_message.ParseFromString(response_msg);
    }
    catch(std::exception& ex){
        std::cerr << ex.what() << " in EnvironmentRequester::AddDeployment" << std::endl;
        throw std::runtime_error("Failed to parse message in EnvironmentRequester::AddDeployment");
    }

    std::string port_string = reply_message.logger().publisher_port();

    return port_string;
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
    ZMQSendRequest(*update_socket_, request.request_data_);
    auto reply = ZMQReceiveReply(*update_socket_);

    //Handle response
    request.response_->set_value(reply);
}

void EnvironmentRequester::ZMQSendRequest(zmq::socket_t& socket, const std::string& request){
    std::string lamport_string = std::to_string(Tick());
    zmq::message_t lamport_time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t request_msg(request.begin(), request.end());

    try{
        socket.send(lamport_time_msg, ZMQ_SNDMORE);
        socket.send(request_msg);
    }
    catch(std::exception error){
        //TODO: Handle this
        std::cerr << error.what() << "in EnvironmentRequester::ZMQSendTwoPartRequest" << std::endl;
    }
}

std::string EnvironmentRequester::ZMQReceiveReply(zmq::socket_t& socket){
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;

    std::vector<zmq::pollitem_t> poll_items = {{socket, 0, ZMQ_POLLIN, 0}};

    int events = zmq::poll(poll_items, REQUEST_TIMEOUT);

    if(end_flag_){
        return "";
    }

    if(events >= 1){
        try{
            socket.recv(&lamport_time_msg);
            socket.recv(&request_contents_msg);
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
    else{
        std::cerr << "Communication with environment manager timed out" << std::endl;
        return "";
    }
}

void EnvironmentRequester::HandleReply(NodeManager::EnvironmentMessage& message){
    switch(message.type()){
        case NodeManager::EnvironmentMessage::HEARTBEAT_ACK:{
            //NO-OP
            break;
        }
        case NodeManager::EnvironmentMessage::UPDATE_DEPLOYMENT:{
            //TODO:
            //Parse control message and give to node manager to update it's endpoint
            
            break;
        }
        default:{
            throw std::runtime_error("Got invalid reply type from environment manager.");
        }

    }
    return;
}

