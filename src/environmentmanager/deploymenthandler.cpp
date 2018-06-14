#include "deploymenthandler.h"
#include <iostream>

#include "deploymentgenerator.h"
#include "deploymentrules/zmq/zmqrule.h"
#include "deploymentrules/dds/ddsrule.h"
#include "deploymentrules/amqp/amqprule.h"

DeploymentHandler::DeploymentHandler(Environment& env,
                                    zmq::context_t& context,
                                    const std::string& ip_addr,
                                    Environment::DeploymentType deployment_type,
                                    const std::string& deployment_ip_address,
                                    std::promise<std::string>* port_promise,
                                    const std::string& experiment_id) :
                                    environment_(env),
                                    context_(context){

    ip_addr_ = ip_addr;
    deployment_type_ = deployment_type;
    deployment_ip_address_ = deployment_ip_address;
    port_promise_ = port_promise;
    experiment_id_ = experiment_id;
    handler_thread_ = std::unique_ptr<std::thread>(new std::thread(&DeploymentHandler::HeartbeatLoop, this));
}

void DeploymentHandler::Terminate(){
    //TODO: send terminate messages to node manager attached to this thread.
    handler_thread_->join();
}

void DeploymentHandler::HeartbeatLoop() noexcept{
    auto handler_socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context_, ZMQ_REP));

    time_added_ = environment_.GetClock();

    std::string assigned_port = environment_.AddDeployment(experiment_id_, deployment_ip_address_, deployment_type_);
    try{
        handler_socket->bind(TCPify(ip_addr_, assigned_port));
        port_promise_->set_value(assigned_port);

    }catch(zmq::error_t ex){
        //Set our promise as exception and exit if we can't find a free port.
        port_promise_->set_exception(std::current_exception());
        RemoveDeployment(time_added_);
        return;
    }

    //Initialise our poll item list
    std::vector<zmq::pollitem_t> sockets = {{*handler_socket, 0, ZMQ_POLLIN, 0}};

    //Wait for first heartbeat, allow more time for this one in case of server congestion
    if(zmq::poll(sockets, INITIAL_TIMEOUT) >= 1){
        try{
            auto initial_request = ZMQReceiveRequest(*handler_socket);
            auto initial_message = HandleRequest(initial_request);
            ZMQSendReply(*handler_socket, initial_message);
        }
        catch(const zmq::error_t& exception){
            std::cerr << "Exception in DeploymentHandler::HeartbeatLoop (initial): " << exception.what() << std::endl;
            RemoveDeployment(time_added_);
            return;
        }
        
    }
    //Break out early if we never get our first heartbeat
    else{
        RemoveDeployment(time_added_);
        return;
    }

    int interval = INITIAL_INTERVAL;
    int liveness = HEARTBEAT_LIVENESS;

    while(!removed_flag_){
        //Poll zmq socket for heartbeat message, time out after HEARTBEAT_TIMEOUT milliseconds
        if(zmq::poll(sockets, HEARTBEAT_INTERVAL) >= 1){
            //Reset
            liveness = HEARTBEAT_LIVENESS;
            interval = INITIAL_INTERVAL;

            try{
                auto request = ZMQReceiveRequest(*handler_socket);
                auto message = HandleRequest(request);
                ZMQSendReply(*handler_socket, message);
            }
            catch(const zmq::error_t& exception){
                std::cerr << "Exception in DeploymentHandler::HeartbeatLoop(rec): " << exception.what() << std::endl;
                break;
            }
            //TODO: Update experiments status to be ACTIVE
        }
        else if(--liveness == 0){
            //TODO: Update experiments status to be TIMEOUT
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(interval < MAX_INTERVAL){
                interval *= 2;
            }
            else{
                //Timed out, break out and remove deployment
                break;
            }
            liveness = HEARTBEAT_LIVENESS;
        }
    }
    RemoveDeployment(time_added_);
}

void DeploymentHandler::RemoveDeployment(uint64_t call_time) noexcept{
    try{
        if(!removed_flag_){
            if(deployment_type_ == Environment::DeploymentType::EXECUTION_MASTER){
                environment_.RemoveExperiment(experiment_id_, call_time);
            }
            if(deployment_type_ == Environment::DeploymentType::LOGAN_CLIENT){
                environment_.RemoveLoganClient(experiment_id_, deployment_ip_address_);
            }
            removed_flag_ = true;
        }
    }
    catch(...){
        std::cerr << "Exception in DeploymentHandler::RemoveDeployment" << std::endl;
    }
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}

void DeploymentHandler::ZMQSendReply(zmq::socket_t& socket, const std::string& message){
    std::string lamport_string = std::to_string(environment_.Tick());
    zmq::message_t time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t msg(message.begin(), message.end());

    socket.send(time_msg, ZMQ_SNDMORE);
    socket.send(msg);
}

std::pair<uint64_t, std::string> DeploymentHandler::ZMQReceiveRequest(zmq::socket_t& socket){
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;
    socket.recv(&lamport_time_msg);
    socket.recv(&request_contents_msg);
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());

    uint64_t lamport_time = environment_.SetClock(std::stoull(incoming_time));
    return std::make_pair(lamport_time, contents);
}

std::string DeploymentHandler::HandleRequest(std::pair<uint64_t, std::string> request){
    auto message_time = request.first;
    NodeManager::EnvironmentMessage message;
    message.ParseFromString(request.second);

    try{
        switch(message.type()){

            case NodeManager::EnvironmentMessage::GET_DEPLOYMENT_INFO:{
                //Create generator and populate message
                DeploymentGenerator generator(environment_);
                generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Zmq::DeploymentRule(environment_)));
                generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Dds::DeploymentRule(environment_)));
                generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Amqp::DeploymentRule(environment_)));
                generator.PopulateDeployment(*(message.mutable_control_message()));
                message.set_type(NodeManager::EnvironmentMessage::SUCCESS);
                break;
            }

            case NodeManager::EnvironmentMessage::REMOVE_DEPLOYMENT:{
                RemoveDeployment(message_time);
                message.set_type(NodeManager::EnvironmentMessage::SUCCESS);
                break;
            }

            case NodeManager::EnvironmentMessage::REMOVE_LOGAN_CLIENT:{
                
            }

            case NodeManager::EnvironmentMessage::HEARTBEAT:{
                if(environment_.ExperimentIsDirty(experiment_id_)){
                    HandleDirtyExperiment(message);
                }else{
                    message.set_type(NodeManager::EnvironmentMessage::HEARTBEAT_ACK);
                }
                break;
            }

            case NodeManager::EnvironmentMessage::LOGAN_QUERY:{
                HandleLoganQuery(message);
                break;
            }

            default:{
                std::cerr << message.DebugString() << std::endl;
                message.set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
                break;
            }
        }
    }
    catch(std::exception& ex){
        //TODO: Add ex.what() as error message.
        message.set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
    }

    return message.SerializeAsString();

}

void DeploymentHandler::HandleDirtyExperiment(NodeManager::EnvironmentMessage& message){
    
    message.set_type(NodeManager::EnvironmentMessage::UPDATE_DEPLOYMENT);

    auto update_message = message.mutable_control_message();

    environment_.GetExperimentUpdate(experiment_id_, *update_message);

}

void DeploymentHandler::HandleLoganQuery(NodeManager::EnvironmentMessage& message){
    auto logger_message = message.mutable_logger(0);
    auto ip_address = logger_message->publisher_address();
    auto logger_port = environment_.GetLoganPublisherPort(experiment_id_, ip_address);
    logger_message->set_publisher_port(logger_port);

    message.set_type(NodeManager::EnvironmentMessage::LOGAN_RESPONSE);
}