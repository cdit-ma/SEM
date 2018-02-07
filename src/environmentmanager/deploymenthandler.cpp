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
        auto request = ReceiveTwoPartRequest(handler_socket_);
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
            auto request = ReceiveTwoPartRequest(handler_socket_);
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

void DeploymentHandler::SendTwoPartReply(zmq::socket_t* socket, const std::string& part_one,
                                                                 const std::string& part_two){
    std::string lamport_string = std::to_string(environment_->Tick());
    zmq::message_t time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t part_one_msg(part_one.begin(), part_one.end());
    zmq::message_t part_two_msg(part_two.begin(), part_two.end());

    try{
        socket->send(part_one_msg, ZMQ_SNDMORE);
        socket->send(time_msg, ZMQ_SNDMORE);
        socket->send(part_two_msg);
    }
    catch(std::exception e){
        std::cout << e.what() << " in DeploymentHandler::SendTwoPartReply" << std::endl;
    }
}

std::tuple<std::string, long, std::string> DeploymentHandler::ReceiveTwoPartRequest(zmq::socket_t* socket){
    zmq::message_t request_type_msg;
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;
    try{
        socket->recv(&request_type_msg);
        socket->recv(&lamport_time_msg);
        socket->recv(&request_contents_msg);
    }
    catch(zmq::error_t error){
        //TODO: Throw this further up
        std::cout << error.what()  << " in DeploymentHandler::ReceiveTwoPartRequest" << std::endl;
    }
    std::string type(static_cast<const char*>(request_type_msg.data()), request_type_msg.size());
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());

    long lamport_time = environment_->SetClock(std::stol(incoming_time));
    return std::make_tuple(type, lamport_time, contents);
}

void DeploymentHandler::HandleRequest(std::tuple<std::string, long, std::string> request){

    std::string ack_string("ACK");

    if(std::get<0>(request).compare("HEARTBEAT") == 0){
        SendTwoPartReply(handler_socket_, ack_string, "");
    }

    else if(std::get<0>(request).compare("ASSIGNMENT_REQUEST") == 0){
        std::string component_id = std::get<2>(request);
        std::string port_string = environment_->AddComponent(deployment_id_, component_id, component_id, time_added_);
        port_map_[component_id] = port_string;
        std::string response(port_string);

        SendTwoPartReply(handler_socket_, "SUCCESS", response);
    }

    else if(std::get<0>(request).compare("UPDATE") == 0){
        //TODO: Handle update
        SendTwoPartReply(handler_socket_, ack_string, "Got update");
    }

    else if(std::get<0>(request).compare("SHUTDOWN_DEPLOYMENT") == 0){
        //TODO: Handle end of deployment.
        SendTwoPartReply(handler_socket_, ack_string, "SHUTDOWN_DEPLOYMENT");
    }

    else{
        SendTwoPartReply(handler_socket_, "ERROR", "Unknown heartbeat message type.");
    }
}
