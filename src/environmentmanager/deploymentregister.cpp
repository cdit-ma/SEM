#include "deploymentregister.h"
#include <iostream>
#include <chrono>
#include <exception>

#include "controlmessage.pb.h"

DeploymentRegister::DeploymentRegister(const std::string& ip_addr, const std::string& registration_port, 
                                        int portrange_min, int portrange_max){

    assert(portrange_min < portrange_max);
    ip_addr_ = ip_addr;
    registration_port_ = registration_port;

    context_ = new zmq::context_t(1);
    environment_ = new Environment(portrange_min, portrange_max);

}

void DeploymentRegister::Start(){
    registration_loop_ = new std::thread(&DeploymentRegister::RegistrationLoop, this);
}

//Main registration loop, passes request workload off to other threads
void DeploymentRegister::RegistrationLoop() noexcept{
    zmq::socket_t* rep;
    try{
        rep = new zmq::socket_t(*context_, ZMQ_REP);
        rep->bind(TCPify(ip_addr_, registration_port_));
    }
    catch(zmq::error_t& e){
        std::cerr << "Could not bind reply socket in registration loop. IP_ADDR: " << ip_addr_ <<
                                        " Port: " << registration_port_ << std::endl;
        return;
    }

    while(true){
        //Receive deployment information
        auto reply = ZMQReceiveRequest(rep);

        NodeManager::EnvironmentMessage message;
        bool parse_success = message.ParseFromString(reply.second);

        if(parse_success){
            try{
                RequestHandler(message);
            }
            catch(const std::exception& exception){
                std::cerr << "Exception when handling request in DeploymentRegister::RegistrationLoop: " << exception.what() << std::endl;
                message.set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
            }
        }

        ZMQSendReply(rep, message.SerializeAsString());
    }
}

void DeploymentRegister::RequestHandler(NodeManager::EnvironmentMessage& message){
    switch(message.type()){
        case NodeManager::EnvironmentMessage::ADD_DEPLOYMENT:{
            HandleAddDeployment(message);
            break;
        }

        case NodeManager::EnvironmentMessage::NODE_QUERY:{
            HandleNodeQuery(message);
            break;
        }

        default:{
            throw std::runtime_error("Unrecognised message type in DeploymentRegister::RequestHandler.");
            break;
        }
    }
}

void DeploymentRegister::HandleAddDeployment(NodeManager::EnvironmentMessage& message){
    //Push work onto new thread with port number promise
    std::promise<std::string>* port_promise = new std::promise<std::string>();
    std::future<std::string> port_future = port_promise->get_future();
    std::string port;

    auto deployment_handler = new DeploymentHandler(environment_, context_, ip_addr_, port_promise, message.experiment_id());
    deployments_.push_back(deployment_handler);

    try{
        //Wait for port assignment from heartbeat loop, .get() will throw if out of ports.
        port = port_future.get();
        message.set_update_endpoint(TCPify(ip_addr_, port));
    }
    catch(const std::exception& e){
        std::cerr << "Exception: " << e.what() << " (Probably out of ports)" << std::endl;
        std::string error_msg("Exception thrown by deployment register: ");
        error_msg += e.what();

        throw std::runtime_error(error_msg);
    }
}

void DeploymentRegister::HandleNodeQuery(NodeManager::EnvironmentMessage& message){
    std::string experiment_id = message.experiment_id();

    if(message.control_message().nodes_size() < 1){
        throw std::runtime_error("No nodes supplied in node query.");
    }

    auto control_message = message.mutable_control_message();
    auto node = message.mutable_control_message()->mutable_nodes(0);

    std::string ip_address;

    for(int i = 0; i < node->attributes_size(); i++){
        auto attribute = node->attributes(i);
        if(attribute.info().name() == "ip_address"){
            ip_address = attribute.s(0);
        }
    }

    if(ip_address.empty()){
        throw std::runtime_error("No ip_address set in message passed to HandleNodeQuery.");
    }

    if(environment_->NodeDeployedTo(experiment_id, ip_address)){
        //Have experiment_id in environment, and ip_addr has component deployed to id
        std::string management_port = environment_->GetNodeManagementPort(experiment_id, ip_address);
        std::string model_logger_port = environment_->GetNodeModelLoggerPort(experiment_id, ip_address);

        auto management_attribute = node->add_attributes();
        auto management_attribute_info = management_attribute->mutable_info();
        management_attribute_info->set_name("management_port");
        management_attribute->set_kind(NodeManager::Attribute::STRING);
        management_attribute->add_s(management_port);

        auto modellogger_attribute = node->add_attributes();
        auto modellogger_attribute_info = modellogger_attribute->mutable_info();
        modellogger_attribute_info->set_name("modellogger_port");
        modellogger_attribute->set_kind(NodeManager::Attribute::STRING);
        modellogger_attribute->add_s(model_logger_port);

        message.set_type(NodeManager::EnvironmentMessage::SUCCESS);
        control_message->set_type(NodeManager::ControlMessage::CONFIGURE);
    }
    else if(!environment_->ModelNameExists(experiment_id)){
        //Dont have an experiment with this id, send back a no_type s.t. client re-trys in a bit
        message.set_type(NodeManager::EnvironmentMessage::SUCCESS);
        control_message->set_type(NodeManager::ControlMessage::NO_TYPE);
    }
    else{
        //At this point, we have an experiment of the same id as ours and we aren't deployed to it.
        //Therefore terminate
        message.set_type(NodeManager::EnvironmentMessage::SUCCESS);
        control_message->set_type(NodeManager::ControlMessage::TERMINATE);
    }
}

std::string DeploymentRegister::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);

}

std::string DeploymentRegister::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}

void DeploymentRegister::ZMQSendReply(zmq::socket_t* socket, const std::string& message){
    std::string lamport_string = std::to_string(environment_->Tick());
    zmq::message_t lamport_time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t zmq_msg(message.begin(), message.end());

    try{
        socket->send(lamport_time_msg, ZMQ_SNDMORE);
        socket->send(zmq_msg);
    }
    catch(std::exception error){
        std::cerr << error.what() << "in DeploymentRegister::SendTwoPartReply" << std::endl;
    }
}

std::pair<uint64_t, std::string> DeploymentRegister::ZMQReceiveRequest(zmq::socket_t* socket){
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;
    try{
        socket->recv(&lamport_time_msg);
        socket->recv(&request_contents_msg);
    }
    catch(zmq::error_t error){
        //TODO: Throw this further up
        std::cerr << error.what() << "in DeploymentRegister::ReceiveTwoPartRequest" << std::endl;
    }
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());
    uint64_t lamport_time = environment_->SetClock(std::stoull(incoming_time));

    return std::make_pair(lamport_time, contents);
}
