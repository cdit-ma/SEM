#include "deploymenthandler.h"
#include <iostream>

#include "environmentmessage/environmentmessage.pb.h"

DeploymentHandler::DeploymentHandler(Environment* env, zmq::context_t* context, const std::string& ip_addr, 
                                    std::promise<std::string>* port_promise, const std::string& deployment_id){

    environment_ = env;
    context_ = context;
    ip_addr_ = ip_addr;
    deployment_id_ = deployment_id;
    port_promise_ = port_promise;
    handler_thread_ = new std::thread(&DeploymentHandler::Init, this);
}

void DeploymentHandler::Init(){
    handler_socket_ = new zmq::socket_t(*context_, ZMQ_REP);

    time_added_ = environment_->GetClock();

    std::string assigned_port = environment_->AddDeployment(deployment_id_, deployment_id_, time_added_);
    try{
        handler_socket_->bind(TCPify(ip_addr_, assigned_port));

        port_promise_->set_value(assigned_port);

    }catch(zmq::error_t ex){
        //Set our promise as exception and exit if we can't find a free port.
        port_promise_->set_exception(std::current_exception());
        return;
    }

    //Start heartbeat to track liveness of deployment
    //Use heartbeat to receive any updates re. components addition/removal
    HeartbeatLoop();
}

void DeploymentHandler::HeartbeatLoop(){
    //Initialise our poll item list
    std::vector<zmq::pollitem_t> sockets = {{*handler_socket_, 0, ZMQ_POLLIN, 0}};

    //Wait for first heartbeat, allow more time for this one in case of server congestion
    int initial_events = zmq::poll(sockets, INITIAL_TIMEOUT);

    if(initial_events >= 1){
        auto request = ZMQReceiveRequest(handler_socket_);
        HandleRequest(request);
    }
    //Break out early if we never get our first heartbeat
    else{
        delete handler_socket_;
        RemoveDeployment(time_added_);
        return;
    }

    int interval = INITIAL_INTERVAL;
    int liveness = HEARTBEAT_LIVENESS;

    while(!removed_flag_){
        zmq::message_t hb_message;

        //Poll zmq socket for heartbeat message, time out after HEARTBEAT_TIMEOUT milliseconds
        int events = zmq::poll(sockets, HEARTBEAT_INTERVAL);
        if(removed_flag_){
            break;
        }

        if(events >= 1){
            //Reset
            liveness = HEARTBEAT_LIVENESS;
            interval = INITIAL_INTERVAL;
            auto request = ZMQReceiveRequest(handler_socket_);
            HandleRequest(request);
            if(removed_flag_){
                break;
            }
            environment_->DeploymentLive(deployment_id_, time_added_);
        }
        else if(--liveness == 0){
            environment_->DeploymentTimeout(deployment_id_, time_added_);
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(interval < MAX_INTERVAL){
                interval *= 2;
            }
            else{
                RemoveDeployment(time_added_);
                break;
            }
            liveness = HEARTBEAT_LIVENESS;
        }
    }
    //Broken out of heartbeat loop at this point, remove deployment
    delete handler_socket_;
}

void DeploymentHandler::RemoveDeployment(uint64_t call_time){
    environment_->RemoveDeployment(deployment_id_, call_time);
    removed_flag_ = true;
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}

void DeploymentHandler::ZMQSendReply(zmq::socket_t* socket, const std::string& message){
    std::string lamport_string = std::to_string(environment_->Tick());
    zmq::message_t time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t msg(message.begin(), message.end());

    try{
        socket->send(time_msg, ZMQ_SNDMORE);
        socket->send(msg);
    }
    catch(std::exception e){
        std::cerr << e.what() << " in DeploymentHandler::ZMQSendReply" << std::endl;
    }
}

std::pair<uint64_t, std::string> DeploymentHandler::ZMQReceiveRequest(zmq::socket_t* socket){
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;
    try{
        socket->recv(&lamport_time_msg);
        socket->recv(&request_contents_msg);
    }
    catch(zmq::error_t error){
        //TODO: Throw this further up
        std::cerr << error.what() << " in DeploymentHandler::ZMQReceiveRequest" << std::endl;
    }
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());

    uint64_t lamport_time = environment_->SetClock(std::stoull(incoming_time));
    return std::make_pair(lamport_time, contents);
}

void DeploymentHandler::HandleRequest(std::pair<uint64_t, std::string> request){
    auto message_time = request.first;
    EnvMessage message;
    message.ParseFromString(request.second);

    switch(message.type()){
        case EnvMessage::HEARTBEAT:{
            message.set_type(EnvMessage::HEARTBEAT_ACK);
            ZMQSendReply(handler_socket_, message.SerializeAsString());
            break;
        }
        case EnvMessage::ADD_DEPLOYMENT:{
            auto reply = HandleAddDeployment(message_time, message);
            ZMQSendReply(handler_socket_, reply.SerializeAsString());
            break;
        }
        case EnvMessage::GET_DEPLOYMENT_INFO:{
            auto reply = HandleGetDeploymentInfo(message_time, message);
            ZMQSendReply(handler_socket_, reply.SerializeAsString());
            break;
        }
        case EnvMessage::REMOVE_DEPLOYMENT:{
            auto reply = HandleRemoveDeployment(message_time, message);
            ZMQSendReply(handler_socket_, reply.SerializeAsString());
            break;
        }
        case EnvMessage::ADD_COMPONENT:{
            auto reply = HandleAddComponent(message_time, message);
            ZMQSendReply(handler_socket_, reply.SerializeAsString());
            break;
        }
        case EnvMessage::REMOVE_COMPONENT:{
            auto reply = HandleRemoveComponent(message_time, message);
            ZMQSendReply(handler_socket_, reply.SerializeAsString());
            break;
        }
        case EnvMessage::ADD_ENDPOINT:{
            message.set_type(EnvMessage::ERROR_RESPONSE);
            ZMQSendReply(handler_socket_, message.SerializeAsString());
            break;
        }
        case EnvMessage::REMOVE_ENDPOINT:{
            message.set_type(EnvMessage::ERROR_RESPONSE);
            ZMQSendReply(handler_socket_, message.SerializeAsString());
            break;
        }
        default:{
            message.set_type(EnvMessage::ERROR_RESPONSE);
            ZMQSendReply(handler_socket_, message.SerializeAsString());
            break;
        }
    }
}

EnvironmentManager::EnvironmentMessage DeploymentHandler::HandleAddDeployment(uint64_t message_time,
                                                            EnvMessage message){
    auto deployment = message.mutable_deployments(0);
    std::string deployment_id = deployment->id();

}

EnvironmentManager::EnvironmentMessage DeploymentHandler::HandleRemoveDeployment(uint64_t message_time,
                                                            EnvMessage message){
    RemoveDeployment(message_time);
    return message;
}

EnvironmentManager::EnvironmentMessage DeploymentHandler::HandleAddComponent(uint64_t message_time,
                                                            EnvMessage message){
    //Handle add component message
    auto endpoint = message.mutable_components(0)->mutable_endpoints(0);
    std::string port_string = environment_->AddComponent(deployment_id_, message.components(0).id(), "", message_time);
    if(port_string.empty()){
        message.set_type(EnvironmentManager::EnvironmentMessage::ERROR_RESPONSE);
    }
    endpoint->set_port(port_string);
    return message;
}

EnvironmentManager::EnvironmentMessage DeploymentHandler::HandleRemoveComponent(uint64_t message_time,
                                                            EnvMessage message){
    //Handle remove component message
    auto component_id = message.components(0).id();
    environment_->RemoveComponent(component_id, message_time);
    message.mutable_components(0)->Clear();
    return message;
}

EnvironmentManager::EnvironmentMessage DeploymentHandler::HandleAddEndpoint(uint64_t message_time,
                                                            EnvMessage message){
    auto endpoint = message.mutable_endpoints(0);

    std::string deployment_id = message.deployments(0).id();

    //std::string port_string = environment_->AddEndpoint(deployment_id, endpoint->id(), message_time);

    std::string port_string;
    endpoint->set_port(port_string);
    return message;
}

EnvironmentManager::EnvironmentMessage DeploymentHandler::HandleGetDeploymentInfo(uint64_t message_time, EnvMessage message){
    auto deployment = message.mutable_deployments(0);
    auto master_endpoint = deployment->add_endpoints();
    auto model_logger_endpoint = deployment->add_endpoints();

    master_endpoint->set_type(EnvironmentManager::Endpoint::MODEL_LOGGER);
    master_endpoint->set_port(environment_->GetMasterPort(deployment_id_));

    model_logger_endpoint->set_type(EnvironmentManager::Endpoint::DEPLOYMENT_MASTER);
    model_logger_endpoint->set_port(environment_->GetModelLoggerPort(deployment_id_));

    return message;
}
