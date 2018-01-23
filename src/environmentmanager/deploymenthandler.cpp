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

    std::string assigned_port = environment_->AddDeployment(deployment_id_);
    try{
        //Bind to random port on local ip address
        handler_socket_->bind(TCPify(ip_addr_, assigned_port));

        port_promise_->set_value(assigned_port);

    }catch(zmq::error_t ex){
        //Set our promise as exception and exit if we can't find a free port.
        port_promise_->set_exception(std::current_exception());
        return;
    }

    //Infinitely loop while deployment requests ports
    while(true){

        auto request = ReceiveTwoPartRequest(handler_socket_);
        std::string request_type = request.first;
        std::string request_contents = request.second;

        if(request_type.compare("ASSIGNMENT_REQUEST") == 0){

            std::string component_id = request_contents;

            std::string header("ASSIGNMENT_REPLY");
            std::string port_string = environment_->AddComponent(component_id);
            port_map_[component_id] = port_string;
            std::string response(port_string);

            SendTwoPartReply(handler_socket_, header, response);
        }

        else if(request_type.compare("END_ASSIGNMENT") == 0){
            std::string header("ASSIGNMENT_REPLY");
            std::string response = "END";

            SendTwoPartReply(handler_socket_, header, response);
            break;
        }

        //Unrecognised message type
        else{
            std::cout << "ERROR: Unrecognised message type passed to DeploymentHandler" << std::endl;
            SendTwoPartReply(handler_socket_, "ERROR", "Unrecognised message type passed to DeploymentHandler");
        }
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

        if(request.first.compare("HEARTBEAT") == 0){
            SendTwoPartReply(handler_socket_, ack_str, "Initial Ack");
        }

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

            if(request.first.compare("HEARTBEAT") == 0){
                SendTwoPartReply(handler_socket_, ack_str, "");
            }

            else if(request.first.compare("UPDATE") == 0){
                SendTwoPartReply(handler_socket_, ack_str, "Got update");
            }

            else{
                SendTwoPartReply(handler_socket_, "ERROR", "Unknown heartbeat message type.");
            }

        }
        else if(--liveness == 0){
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
        environment_->RemoveComponent(element.first);
    }

    environment_->RemoveDeployment(deployment_id_);
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}

void DeploymentHandler::SendTwoPartReply(zmq::socket_t* socket, const std::string& part_one,
                                                                 const std::string& part_two){
    zmq::message_t part_one_msg(part_one.begin(), part_one.end());
    zmq::message_t part_two_msg(part_two.begin(), part_two.end());

    try{
        socket->send(part_one_msg, ZMQ_SNDMORE);
        socket->send(part_two_msg);
    }
    catch(std::exception e){
        std::cout << e.what() << std::endl;
    }
}

std::pair<std::string, std::string> DeploymentHandler::ReceiveTwoPartRequest(zmq::socket_t* socket){
    zmq::message_t request_type_msg;
    zmq::message_t request_contents_msg;
    try{
        socket->recv(&request_type_msg);
        socket->recv(&request_contents_msg);
    }
    catch(zmq::error_t error){
        //TODO: Throw this further up
        std::cout << error.what() << std::endl;
    }
    std::string type(static_cast<const char*>(request_type_msg.data()), request_type_msg.size());
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    return std::make_pair(type, contents);
}
