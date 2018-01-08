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

void DeploymentRegister::AddDeployment(int port_no, const std::string& deployment_info){
    std::cout << "Add deployment: " << deployment_info << " On port: " << port_no << std::endl;
    std::unique_lock<std::mutex> lock(register_mutex_);
    deployments_[port_no] = deployment_info;

    std::thread* hb_thread = new std::thread(&DeploymentRegister::HeartbeatLoop, this, 
                                                port_no, deployment_info);
    hb_threads_.push_back(hb_thread);
}

void DeploymentRegister::RemoveDeployment(const std::string& deployment_info){
    std::unique_lock<std::mutex> lock(register_mutex_);
}

void DeploymentRegister::RemoveDeployment(int port_no){
    std::cout << "Remove deployment on port: " << port_no << std::endl;
    std::unique_lock<std::mutex> lock(register_mutex_);
    deployments_.erase(port_no);
}

std::string DeploymentRegister::GetDeploymentInfo() const{

    std::string out;

    for(auto element : deployments_){
        out += " " + element.second;
    }
    return out;
}

std::string DeploymentRegister::GetDeploymentInfo(const std::string& name) const{
    throw new std::bad_function_call;
    return "";
}

void DeploymentRegister::RegistrationLoop(){

    zmq::socket_t rep(*context_, ZMQ_REP);
    std::string endpoint("tcp://" + ip_addr_ + ":" + std::to_string(registration_port_));
    rep.bind(endpoint);
    
    while(true){
        //Wait for connection

        //Receive deployment information
        zmq::message_t msg_type;
        zmq::message_t msg_contents;
        zmq::message_t reply;

        rep.recv(&msg_type);
        rep.recv(&msg_contents);

        auto msg_type_str = std::string(static_cast<const char*>(msg_type.data()), msg_type.size());
        auto msg_contents_str = std::string(static_cast<const char*>(msg_contents.data()), msg_contents.size());

        if(msg_type_str.compare("DEPLOYMENT") == 0){
            //Push work onto new thread and assign that thread a port number.
            int temp_port_number = current_port_;
            current_port_++;

            AddDeployment(temp_port_number, msg_contents_str);

            //Reply with enpoint to send heartbeats and status updates to
            std::string message_str("tcp://" + ip_addr_ + ":" +  std::to_string(temp_port_number));

            zmq::message_t reply(message_str.begin(), message_str.end());

            try{
                rep.send(reply);
            }catch(zmq::error_t ex){
                std::cout << ex.what() << std::endl;
            }
        }

        if(msg_type_str.compare("QUERY") == 0){
            std::string info = GetDeploymentInfo();
            zmq::message_t reply(info.begin(), info.end());
            rep.send(reply);
        }
    }
}

void DeploymentRegister::QueryLoop(){
    throw new std::bad_function_call;
    while(true){
    }
}

void DeploymentRegister::HeartbeatLoop(int port_no, const std::string& deployment_info){
    zmq::socket_t hb_soc(*context_, ZMQ_REP);

    std::string heartbeat_endpoint("tcp://" + ip_addr_ + ":" + std::to_string(port_no));

    try{
        hb_soc.bind(heartbeat_endpoint);
    }catch(zmq::error_t ex){
        std::cout << ex.what() <<  std::endl;
    }
    std::string ack_str("ACK");

    std::vector<zmq::pollitem_t> sockets;
    zmq::pollitem_t item;
    item.socket = hb_soc;
    item.events = ZMQ_POLLIN;

    sockets.push_back(item);

    //Wait for heartbeats
    while(true){
        zmq::message_t hb_message;

        //Poll zmq socket for heartbeat message, time out after two (2) seconds
        int result = zmq::poll(sockets, 2000);

        if(result < 1){
            break;
        }

        hb_soc.recv(&hb_message);

        zmq::message_t ack(ack_str.begin(), ack_str.end());
        hb_soc.send(ack);
    }
    RemoveDeployment(port_no);
}
