#include "environmentrequester.h"
#include <zmq.hpp>
#include <controlmessage.pb.h>
#include "../zmqutils.hpp"
#include "../protorequester/protorequester.hpp"

#include <sstream>

EnvironmentRequester::EnvironmentRequester(const std::string& manager_address, 
                                            const std::string& experiment_id,
                                            EnvironmentRequester::DeploymentType deployment_type) : 
                                            manager_address_(manager_address),
                                            experiment_id_(experiment_id),
                                            deployment_type_(deployment_type)
{}

EnvironmentRequester::~EnvironmentRequester(){
    Terminate();
}

void EnvironmentRequester::Init(const std::string& manager_endpoint){
    context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
    manager_endpoint_ = manager_endpoint;
}

void EnvironmentRequester::AddUpdateCallback(std::function<void (NodeManager::EnvironmentMessage& environment_message)> callback_func){
    update_callback_ = callback_func;
}

void EnvironmentRequester::SetIPAddress(const std::string& ip_addr){
    ip_address_ = ip_addr;
}

NodeManager::ControlMessage EnvironmentRequester::NodeQuery(const std::string& node_endpoint){
    //Construct query message
    NodeManager::EnvironmentMessage message;
    message.set_experiment_id(experiment_id_);
    message.set_type(NodeManager::EnvironmentMessage::NODE_QUERY);

    auto control_message = message.mutable_control_message();

    auto node = control_message->add_nodes();
    auto info = node->mutable_info();

    auto& attribute = (*node->mutable_attributes())["ip_address"];
    auto attribute_info = attribute.mutable_info();
    attribute.mutable_info()->set_name("ip_address");
    attribute.set_kind(NodeManager::Attribute::STRING);
    attribute.add_s(node_endpoint);


    //Get update endpoint
    //note: This falls out of scope and self destructs at the end of this function,
    //      this is important as we cant destruct our context otherwise

    try{
        auto requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(manager_address_));
        auto reply_future = requester->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                                    ("EnvironmentManagerNodeQuery", message, 3000);
        auto reply = reply_future.get();
        return reply->control_message();

    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error("Environment manager request timed out.");
    }catch(const std::exception& ex){
        std::cerr << "Exception in EnvironmentRequester::NodeQuery" << ex.what() << std::endl;
    }
}


void EnvironmentRequester::Start(){
    try{
        
        heartbeat_future_ = std::async(std::launch::async, &EnvironmentRequester::HeartbeatLoop, this);
        //heartbeat_thread_ = std::unique_ptr<std::thread>(new std::thread(&EnvironmentRequester::HeartbeatLoop, this));
    }
    catch(std::exception& ex){
        std::cout << ex.what() << " in EnvironmentRequester::Start" << std::endl;
    }
}

void EnvironmentRequester::HeartbeatLoop(){

    auto initial_requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(manager_endpoint_));

    //Register this deployment with the environment manager
    NodeManager::EnvironmentMessage initial_message;

    //map our two enums
    switch(deployment_type_){
        case DeploymentType::RE_MASTER:{
            initial_message.set_type(NodeManager::EnvironmentMessage::ADD_DEPLOYMENT);
            break;
        }
        case DeploymentType::LOGAN:{
            initial_message.set_type(NodeManager::EnvironmentMessage::ADD_LOGAN_CLIENT);
            initial_message.set_update_endpoint(ip_address_);
            break;
        }
        default:{
            std::cerr << "Deployment type invalid in EnvironmentRequester::HeartbeatLoop" << std::endl;
            assert(true);
        }
    }

    initial_message.set_experiment_id(experiment_id_);

    auto initial_reply_future = initial_requester->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>("DeploymentRegistration", initial_message, 3000);

    try{
        auto initial_reply_messsage = initial_reply_future.get();
        if(initial_reply_messsage->type() == NodeManager::EnvironmentMessage::ERROR_RESPONSE){
            std::stringstream str_stream;
            const auto& error_list = initial_reply_messsage->error_messages();
            std::for_each(error_list.begin(), error_list.end(), [&str_stream](const std::string& str){str_stream << "* " << str << "\n";});
            {
                std::lock_guard<std::mutex> lock(request_queue_lock_);
                //Clearing the requests
                while(!request_queue_.empty()){
                    auto request = std::move(request_queue_.front());
                    const auto& exception = std::runtime_error("Environment Manager Error: \n" + str_stream.str());
                    request->reply_promise.set_exception(std::make_exception_ptr(exception));
                    request_queue_.pop();
                }
            }
            environment_manager_not_found_ = true;
            return;
        }


        manager_update_endpoint_ = initial_reply_messsage->update_endpoint();
    }catch(const zmq::TimeoutException& ex){
        std::lock_guard<std::mutex> lock(request_queue_lock_);
        //Clear all requests
        while(!request_queue_.empty()){
            auto request = std::move(request_queue_.front());
            request->reply_promise.set_exception(std::make_exception_ptr(std::runtime_error("Environment Manager Not Found")));
            request_queue_.pop();
        }
        environment_manager_not_found_ = true;
        return;
    }catch(const std::exception& ex){

    }


    auto update_requester = std::unique_ptr<zmq::ProtoRequester>(new zmq::ProtoRequester(manager_update_endpoint_));

    int retry_count = 0;
    //Start heartbeat loop
    while(retry_count < 5){

        bool timeout = true;
        std::unique_ptr<EnvironmentRequester::Request> request;
        {
            std::unique_lock<std::mutex> lock(request_queue_lock_);

            //Wait for a message on the queue OR our heartbeat period to time out.
            auto got_message = request_queue_cv_.wait_for(lock, std::chrono::milliseconds(HEARTBEAT_PERIOD), [=](){
                return !request_queue_.empty();
            });

            if(end_flag_ || !context_){
                break;
            }

            //We have a message, so take it off the queue and release mutex
            if(got_message){
                request = std::move(request_queue_.front());
                request_queue_.pop();
            }
        }

        NodeManager::EnvironmentMessage message;
        if(request){
            message = *(request->request);
            //Reset our retry
            retry_count = 0;
        }else{
            message.set_type(NodeManager::EnvironmentMessage::HEARTBEAT);
        }

        try{
            auto reply_future = update_requester->SendRequest<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>
                                                            ("EnvironmentManagerHeartbeat", message, 1000);
            auto reply_message = reply_future.get();
            retry_count = 0;
            HandleReply(*reply_message);
        }catch(const zmq::TimeoutException& ex){
            retry_count ++;
            std::cerr << "Heartbeat timed out: Retry # " << retry_count << std::endl;
        }catch(const std::exception& ex){
            //todo: call registered exception handling callback??
            std::cerr << "EnvironmentRequester::HeartbeatLoop handle reply " << ex.what() << std::endl;
        }
    }

    if(retry_count >= 5){
        std::cerr << "EnvironmentRequester::HeartbeatLoop Timed out" << std::endl;
    }
}

void EnvironmentRequester::Terminate(){
    {
        std::lock_guard<std::mutex> lock(request_queue_lock_);
        end_flag_ = true;
        request_queue_cv_.notify_one();
    }
    if(heartbeat_future_.valid()){
        heartbeat_future_.get();
    }
}

NodeManager::ControlMessage EnvironmentRequester::AddDeployment(const NodeManager::ControlMessage& control_message){
    if(environment_manager_not_found_){
        throw std::runtime_error("Could not add deployment as environment manager was not found.");
    }

    NodeManager::EnvironmentMessage env_message;
    env_message.set_type(NodeManager::EnvironmentMessage::GET_DEPLOYMENT_INFO);
    
    auto current_control_message = env_message.mutable_control_message();
    *current_control_message = control_message;

    try{
        auto reply_future = QueueRequest(env_message);
        auto reply_msg = reply_future.get();
        if(reply_msg.type() != NodeManager::EnvironmentMessage::SUCCESS){
            std::stringstream str_stream;
            const auto& error_list = reply_msg.error_messages();
            std::for_each(error_list.begin(), error_list.end(), [&str_stream](const std::string& str){str_stream << "* " << str << "\n";});
            throw std::runtime_error("Got non success in EnvironmentRequester::AddDeployment: \n" + str_stream.str());
        }
        return reply_msg.control_message();
    }
    catch(std::exception& ex){
        std::cerr << ex.what() << " in EnvironmentRequester::AddDeployment" << std::endl;
        throw std::runtime_error("Failed to parse message in EnvironmentRequester::AddDeployment");
    }

}

void EnvironmentRequester::RemoveDeployment(){
    NodeManager::EnvironmentMessage message;
    if(deployment_type_ == EnvironmentRequester::DeploymentType::RE_MASTER){
        message.set_type(NodeManager::EnvironmentMessage::REMOVE_DEPLOYMENT);
    }

    if(deployment_type_ == EnvironmentRequester::DeploymentType::LOGAN){
        message.set_type(NodeManager::EnvironmentMessage::REMOVE_LOGAN_CLIENT);
    }

    try{
        auto reply_future = QueueRequest(message);
        auto reply_msg = reply_future.get();
        if(reply_msg.type() == NodeManager::EnvironmentMessage::SUCCESS){
            std::cout << "* Removed from environment manager." << std::endl;
            Terminate();
        }
    }
    catch(std::exception& ex){
        std::cout << ex.what() << " in EnvironmentRequester::RemoveDeployment" << std::endl;
    }
}

NodeManager::EnvironmentMessage EnvironmentRequester::GetLoganInfo(const std::string& node_ip_address){
    if(environment_manager_not_found_){
        throw std::runtime_error("Could not add deployment as environment manager was not found.");
    }

    NodeManager::EnvironmentMessage request_message;

    request_message.set_type(NodeManager::EnvironmentMessage::LOGAN_QUERY);
    request_message.set_experiment_id(experiment_id_);
    request_message.set_update_endpoint(ip_address_);

    NodeManager::EnvironmentMessage reply_message;

    auto response = QueueRequest(request_message);
    try{
        auto response_msg = response.get();
        return response_msg;
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error("Get Logan Info request timed out.");
    }catch(const std::exception& ex){

    }
}
std::future<NodeManager::EnvironmentMessage> EnvironmentRequester::QueueRequest(const NodeManager::EnvironmentMessage& request){
    // XXX: D:
    auto request_struct = std::unique_ptr<Request>(new Request({std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage(request))}));
    auto request_future = request_struct->reply_promise.get_future();
    
    std::lock_guard<std::mutex> lock(request_queue_lock_);
    request_queue_.emplace(std::move(request_struct));
    request_queue_cv_.notify_one();
    return request_future;
}

void EnvironmentRequester::HandleReply(NodeManager::EnvironmentMessage& message){

    if(message.type() == NodeManager::EnvironmentMessage::HEARTBEAT_ACK){
        //no-op
        return;
    }else{
        if(update_callback_){
            update_callback_(message);
        }else{
            throw std::runtime_error("Update callback not set");
        }
    }
}

