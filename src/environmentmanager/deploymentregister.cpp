#include "deploymentregister.h"
#include <iostream>

DeploymentRegister::DeploymentRegister(const std::string& ip_addr, int registration_port,
                                        int hb_start_port = 22338){
    ip_addr_ = ip_addr;
    registration_port_ = registration_port;

    hb_start_port_ = hb_start_port;
    current_port_ = hb_start_port_;
    context_ = new zmq::context_t(1);
}

void DeploymentRegister::Start(){
    registration_loop_ = new std::thread(&DeploymentRegister::RegistrationLoop, this);
}

void DeploymentRegister::AddDeployment(const std::string& hb_endpoint){

    
}

void DeploymentRegister::RemoveDeployment(){

}

void DeploymentRegister::RegistrationLoop(){

    zmq::socket_t rep(*context_, ZMQ_REP);
    std::string endpoint("tcp://" + ip_addr_ + ":" + std::to_string(registration_port_));
    rep.bind(endpoint);
    
    while(true){
        //Wait for connection

        //Receive deployment information
        zmq::message_t deployment_info;
        rep.recv(&deployment_info);
        auto hb_endpoint = std::string(static_cast<const char*>(deployment_info.data()), deployment_info.size());

        //TODO: Store deployment information
        AddDeployment(hb_endpoint);

        //TODO: Start up heartbeat thread
        //TODO: Mutex current port
        std::thread* hb_thread = new std::thread(&DeploymentRegister::HeartbeatLoop, this, current_port_);

        hb_threads_.push_back(hb_thread);

        //Reply with enpoint to send heartbeats and status updates to
        std::string message_str("tcp://" + ip_addr_ + ":" +  std::to_string(current_port_));
        zmq::message_t reply(message_str.begin(), message_str.end());
        rep.send(reply);
        current_port_++;
    }
}

void DeploymentRegister::QueryLoop(){
    throw new std::bad_function_call;
    while(true){
        //wait for deployment request
    }

}

void DeploymentRegister::HeartbeatLoop(int port_no){
    std::cout << "Gained: " << port_no << std::endl;
    zmq::socket_t hb_soc(*context_, ZMQ_REP);

    std::string heartbeat_endpoint("tcp://" + ip_addr_ + ":" + std::to_string(port_no));

    hb_soc.bind(heartbeat_endpoint);

    std::string ack_str("ACK");

    std::vector<zmq::pollitem_t> sockets;
    zmq::pollitem_t item;
    item.socket = hb_soc;
    item.events = ZMQ_POLLIN;

    sockets.push_back(item);

    while(true){
        zmq::message_t hb_message;

        int result = zmq::poll(sockets, 2000);

        if(result < 1){
            break;
        }

        hb_soc.recv(&hb_message);


        zmq::message_t ack(ack_str.begin(), ack_str.end());
        hb_soc.send(ack);
    }
    //TODO: De register manager here

    std::cout << "Lost: " << port_no << std::endl;
}