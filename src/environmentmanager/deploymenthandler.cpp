#include "deploymenthandler.h"
#include <iostream>

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
        //Bind to random port on local ip address
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

    std::string ack_str("ACK");

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
        RemoveDeployment();
        return;
    }

    int interval = INITIAL_INTERVAL;
    int liveness = HEARTBEAT_LIVENESS;

    while(true){
        zmq::message_t hb_message;

        //Poll zmq socket for heartbeat message, time out after HEARTBEAT_TIMEOUT milliseconds
        int events = zmq::poll(sockets, HEARTBEAT_INTERVAL);

        if(events >= 1){
            //Reset
            liveness = HEARTBEAT_LIVENESS;
            interval = INITIAL_INTERVAL;
            auto request = ZMQReceiveRequest(handler_socket_);
            HandleRequest(request);
            environment_->DeploymentLive(deployment_id_, time_added_);
        }
        else if(--liveness == 0){
            environment_->DeploymentTimeout(deployment_id_, time_added_);
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(interval < MAX_INTERVAL){
                interval *= 2;
            }
            else{
                break;
            }
            liveness = HEARTBEAT_LIVENESS;
        }
    }

    //Broken out of heartbeat loop at this point, remove deployment
    delete handler_socket_;
    RemoveDeployment();
}

void DeploymentHandler::RemoveDeployment(){
    for(auto element : port_map_){
        environment_->RemoveComponent(element.first, time_added_);
    }
    environment_->RemoveDeployment(deployment_id_, time_added_);
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
        std::cerr << e.what() << " in DeploymentHandler::SendTwoPartReply" << std::endl;
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

    EnvironmentManager::EnvironmentMessage message;
    message.ParseFromString(request.second);

    switch(message.type()){
        case EnvironmentManager::EnvironmentMessage::HEARTBEAT:{
            ZMQSendReply(handler_socket_, "ack_string");
            break;
        }
        case EnvironmentManager::EnvironmentMessage::ADD_COMPONENT:{
            auto endpoint = message.mutable_components(0)->mutable_endpoints(0);
            std::string port_string = environment_->AddComponent(deployment_id_, message.components(0).id(), "asdf", request.first);

            endpoint->set_port(port_string);
            ZMQSendReply(handler_socket_, message.SerializeAsString());
            break;
        }
        default:{
            ZMQSendReply(handler_socket_, "ERROR");
            break;
        }
    }

}
