#include "deploymentregister.h"
#include <iostream>
#include <chrono>
#include <exception>
#include "../nodemanager/controlmessage/controlmessage.pb.h"

const std::string DeploymentRegister::SUCCESS = "SUCCESS";
const std::string DeploymentRegister::ERROR = "ERROR";

DeploymentRegister::DeploymentRegister(const std::string& ip_addr, const std::string& registration_port){
    ip_addr_ = ip_addr;
    registration_port_ = registration_port;

    context_ = new zmq::context_t(1);
    environment_ = new Environment();

}

void DeploymentRegister::Start(){
    registration_loop_ = new std::thread(&DeploymentRegister::RegistrationLoop, this);
}

//Main registration loop, passes request workload off to other threads
//NOTHROW
void DeploymentRegister::RegistrationLoop(){
    zmq::socket_t* rep = new zmq::socket_t(*context_, ZMQ_REP);
    try{
        rep->bind(TCPify(ip_addr_, registration_port_));
    }
    catch(zmq::error_t& e){
        throw new std::invalid_argument("Could not bind reply socket in registration loop. IP_ADDR: " + ip_addr_ +
                                        " Port: " + registration_port_);
    }
    
    while(true){
        //Receive deployment information
        auto reply = ReceiveTwoPartRequest(rep);

        auto request_type = std::get<0>(reply);
        auto request_contents = std::get<2>(reply);

        //Handle new deployment manager contact
        if(request_type.compare("DEPLOYMENT") == 0){
            //Push work onto new thread with port number promise
            std::promise<std::string>* port_promise = new std::promise<std::string>();
            std::future<std::string> port_future = port_promise->get_future();
            std::string port;

            auto deployment_handler = new DeploymentHandler(environment_, context_, ip_addr_, port_promise, request_contents);
            deployments_.push_back(deployment_handler);
            try{
                //Wait for port assignment from heartbeat loop, .get() will throw if out of ports.
                port = port_future.get();
            }
            catch(const std::exception& e){
                std::cout << "Exception: " << e.what() << " (Probably out of ports)" << std::endl;

                std::string error_msg("Exception thrown by deployment register: ");
                error_msg += e.what();

                SendTwoPartReply(rep, ERROR, error_msg);

                //Continue with no state change
                continue;
            }

            //Reply with enpoint to send heartbeats and status updates to
            SendTwoPartReply(rep, SUCCESS, TCPify(ip_addr_, port));
        }

        else if(request_type.compare("QUERY") == 0){
            std::string response = "";
            SendTwoPartReply(rep, SUCCESS, response);
        }

        else{
            std::cout << "Recieved unknown request type: " << request_type << std::endl;
            SendTwoPartReply(rep, ERROR, "Unknown request type");
        }
    }
}

std::string DeploymentRegister::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);

}

std::string DeploymentRegister::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}

void DeploymentRegister::SendTwoPartReply(zmq::socket_t* socket, const std::string& part_one,
                                                                 const std::string& part_two){
    std::string lamport_string = std::to_string(environment_->Tick());
    zmq::message_t lamport_time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t part_one_msg(part_one.begin(), part_one.end());
    zmq::message_t part_two_msg(part_two.begin(), part_two.end());

    try{
        socket->send(part_one_msg, ZMQ_SNDMORE);
        socket->send(lamport_time_msg, ZMQ_SNDMORE);
        socket->send(part_two_msg);
    }
    catch(std::exception error){
        std::cout << error.what() << "in DeploymentRegister::SendTwoPartReply" << std::endl;
    }
}

std::tuple<std::string, long, std::string> DeploymentRegister::ReceiveTwoPartRequest(zmq::socket_t* socket){
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
        std::cout << error.what() << "in DeploymentRegister::ReceiveTwoPartRequest" << std::endl;
    }
    std::string type(static_cast<const char*>(request_type_msg.data()), request_type_msg.size());
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());
    long lamport_time = environment_->SetClock(std::stol(incoming_time));

    return std::make_tuple(type, lamport_time, contents);
}
