#include "deploymentregister.h"
#include <iostream>
#include <chrono>
#include <exception>
#include <zmq.hpp>

DeploymentRegister::DeploymentRegister(Execution& exe, const std::string& ip_addr, const std::string& registration_port, 
                                        int portrange_min, int portrange_max) : execution_(exe){

    assert(portrange_min < portrange_max);
    ip_addr_ = ip_addr;
    registration_port_ = registration_port;

    context_ = std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
    environment_ = std::unique_ptr<EnvironmentManager::Environment>(new EnvironmentManager::Environment(ip_addr, portrange_min, portrange_max));

    execution_.AddTerminateCallback(std::bind(&DeploymentRegister::Terminate, this));

}

void DeploymentRegister::Start(){
    registration_loop_ = std::thread(&DeploymentRegister::RegistrationLoop, this);
}

void DeploymentRegister::Terminate(){
    context_.reset();
    for(const auto& deployment : deployments_){
        deployment->Terminate();
    }

    for(const auto& client : logan_clients_){
        client->Terminate();
    }
    registration_loop_.join();
}

//Main registration loop, passes request workload off to other threads
void DeploymentRegister::RegistrationLoop() noexcept{
    std::unique_ptr<zmq::socket_t> rep;
    try{
        rep = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(*context_.get(), ZMQ_REP));
        rep->bind(TCPify(ip_addr_, registration_port_));
    }
    catch(zmq::error_t& e){
        std::cerr << "Could not bind reply socket in registration loop. IP_ADDR: " << ip_addr_ <<
                                        " Port: " << registration_port_ << std::endl;
        execution_.Interrupt();
        return;
    }

    while(!terminate_){

        std::pair<uint64_t, std::string> reply;
        //Receive deployment information
        try{
            reply = ZMQReceiveRequest(*rep);
        }catch(const zmq::error_t& exception){
            std::cerr << "Exception in deploymentregister::RegistrationLoop " << exception.what() << std::endl;
            break;
        }

        NodeManager::EnvironmentMessage message;
        bool parse_success = message.ParseFromString(reply.second);

        if(parse_success){
            try{
                //Handle message. Reply message is created by mutating this message.
                RequestHandler(message);
            }
            catch(const zmq::error_t& exception){
                std::cerr << "Exception in deploymentRegister loop: " << exception.what() << std::endl;
                break;
            }
            catch(const std::exception& exception){
                //Print error message to cerr and add to error message field of response.
                //Set response type to ERROR
                std::string error_string = "Exception when handling request in DeploymentRegister::RegistrationLoop: ";
                error_string += exception.what();
                std::cerr << error_string << std::endl;
                message.set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
                message.add_error_messages(error_string);
            }
        }

        try{
            ZMQSendReply(*rep, message.SerializeAsString());
        }
        catch(const zmq::error_t& exception){
            std::cerr << "Exception in deploymentRegister loop: " << exception.what() << std::endl;
            break;
        }
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

        case NodeManager::EnvironmentMessage::ADD_LOGAN_CLIENT:{
            HandleAddLoganClient(message);
            break;
        }

        case NodeManager::EnvironmentMessage::LOGAN_CLIENT_LIST_QUERY:{
            HandleLoganClientListQuery(message);
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
    auto port_promise = std::unique_ptr<std::promise<std::string>> (new std::promise<std::string>());
    std::future<std::string> port_future = port_promise->get_future();
    std::string port;
    std::cerr << "ADDING: " << ip_addr_ << " 2:" << message.update_endpoint() << std::endl;
    deployments_.emplace_back(new DeploymentHandler(*environment_,
                                                    *context_,
                                                    ip_addr_,
                                                    EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER,
                                                    message.update_endpoint(),
                                                    port_promise.get(),
                                                    message.experiment_id()));

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

void DeploymentRegister::HandleAddLoganClient(NodeManager::EnvironmentMessage& message){
    std::string experiment_id = message.experiment_id();

    auto port_promise = std::unique_ptr<std::promise<std::string>> (new std::promise<std::string>());
    std::future<std::string> port_future = port_promise->get_future();
    std::string port;
    logan_clients_.emplace_back(new DeploymentHandler(*environment_,
                                                    *context_,
                                                    ip_addr_,
                                                    EnvironmentManager::Environment::DeploymentType::LOGAN_CLIENT,
                                                    "",
                                                    port_promise.get(),
                                                    experiment_id));

    try{
        port = port_future.get();
        message.set_type(NodeManager::EnvironmentMessage::SUCCESS);
        message.set_update_endpoint(TCPify(ip_addr_, port));
    }
    catch(const std::exception& ex){
        std::cerr << "Exception: " << ex.what() << " (Probably out of ports)" << std::endl;
        std::string error_msg("Exception thrown by deployment register: ");
        error_msg += ex.what();

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
        std::string master_publisher_endpoint = environment_->GetMasterPublisherAddress(experiment_id);
        std::string master_registration_endpoint = environment_->GetMasterRegistrationAddress(experiment_id);

        std::cerr << "master_publisher_endpoint: " << master_publisher_endpoint << std::endl;
        std::cerr << "master_registration_endpoint: " << master_registration_endpoint << std::endl;

        auto master_publisher_attribute = node->add_attributes();
        auto master_publisher_attribute_info = master_publisher_attribute->mutable_info();
        master_publisher_attribute_info->set_name("master_publisher_endpoint");
        master_publisher_attribute->set_kind(NodeManager::Attribute::STRING);
        master_publisher_attribute->add_s(master_publisher_endpoint);

        auto master_registration_attribute = node->add_attributes();
        auto master_registration_attribute_info = master_registration_attribute->mutable_info();
        master_registration_attribute_info->set_name("master_registration_endpoint");
        master_registration_attribute->set_kind(NodeManager::Attribute::STRING);
        master_registration_attribute->add_s(master_registration_endpoint);

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

void DeploymentRegister::HandleLoganClientListQuery(NodeManager::EnvironmentMessage& message){
    // std::string experiment_id = message.experiment_id();

    // auto client_addresses = environment_->GetLoganClientList(experiment_id);

    // for(const auto& client_address : client_addresses){
    //     auto logger = message.add_logger();
    //     logger->set_publisher_address(client_address);
    // }
}

std::string DeploymentRegister::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);

}

std::string DeploymentRegister::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}

void DeploymentRegister::ZMQSendReply(zmq::socket_t& socket, const std::string& message){
    std::string lamport_string = std::to_string(environment_->Tick());
    zmq::message_t lamport_time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t zmq_msg(message.begin(), message.end());

    try{
        socket.send(lamport_time_msg, ZMQ_SNDMORE);
        socket.send(zmq_msg);
    }
    catch(const zmq::error_t& error){
        std::cerr << error.what() << " in DeploymentRegister::SendTwoPartReply" << std::endl;
    }
}

std::pair<uint64_t, std::string> DeploymentRegister::ZMQReceiveRequest(zmq::socket_t& socket){
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;
    socket.recv(&lamport_time_msg);
    socket.recv(&request_contents_msg);
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());
    uint64_t lamport_time = environment_->SetClock(std::stoull(incoming_time));

    return std::make_pair(lamport_time, contents);
}
