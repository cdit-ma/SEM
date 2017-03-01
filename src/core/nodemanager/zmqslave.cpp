#include "zmqslave.h"
#include <zmq.hpp>
#include <iostream>
#include <chrono>

ZMQSlave::ZMQSlave(DeploymentManager* manager, std::string endpoint){
    deployment_manager_ = manager;
    
    //Construct context
    context_ = new zmq::context_t(1);

    //Start the registration thread
    registration_thread_ = new std::thread(&ZMQSlave::RegistrationLoop, this, endpoint);
}

ZMQSlave::~ZMQSlave(){
    //Deleting the context will interupt any blocking ZMQ calls
    if(context_){
        delete context_;    
    }

    if(registration_thread_){
        registration_thread_->join();
        delete registration_thread_;
    }
}

void ZMQSlave::RegistrationLoop(std::string endpoint){
    //Start a request socket, and bind endpoint
    auto socket = zmq::socket_t(*context_, ZMQ_REQ);
    socket.bind(endpoint.c_str());

    //Construct a message to send to the server
    zmq::message_t slave_addr(endpoint.c_str(), endpoint.size());
    zmq::message_t slave_logging_pub_addr;
    zmq::message_t master_control_pub_addr;
    zmq::message_t slave_name;

    try{
        //Send our address to the server, blocks until reply
        socket.send(slave_addr);

        //Get the tcp endpoint for the Publisher
        socket.recv(&master_control_pub_addr);

        //Get the name of this node (For use in filtering)
        socket.recv(&slave_name);

        //Get the tcp endpoint for ModelLogger
        socket.recv(&slave_logging_pub_addr);

        std::string master_control_pub_addr_str(static_cast<char *>(master_control_pub_addr.data()), master_control_pub_addr.size());
        std::string slave_logging_pub_addr_str(static_cast<char *>(slave_logging_pub_addr.data()), slave_logging_pub_addr.size());
        std::string slave_name_str(static_cast<char *>(slave_name.data()), slave_name.size());

        std::cout << "Got Control Publisher End-Point: " << master_control_pub_addr_str << std::endl;
        std::cout << "Got Logger Publisher End-Point: " << slave_logging_pub_addr_str << std::endl;
        std::cout << "Got Slave hostname: " << slave_name_str << std::endl;

        bool success = false;
        std::string reply_message;
        if(deployment_manager_){
            //Setup Model Logging
            bool s1 = deployment_manager_->SetupControlMessageReceiver(master_control_pub_addr_str, slave_name_str);
            bool s2 = deployment_manager_->SetupModelLogger(slave_logging_pub_addr_str, slave_name_str); 

            if(!s1){
                reply_message += "Failed SetupControlMessageReceiver(" + master_control_pub_addr_str + ", " + slave_name_str + ")";
            }
            if(!s2){
                reply_message += "\r\nFailed SetupModelLogger(" + master_control_pub_addr_str + ", " + slave_name_str + ")";
            }
            success = s1 && s2;
        }
        if(success){
            reply_message = "OKAY";
        }

        zmq::message_t slave_response(reply_message.c_str(), reply_message.size());
        //Send our setup response to the server, blocks until reply
        socket.send(slave_response);
    }catch(const zmq::error_t& exception){
        if(exception.num() == ETERM){
            std::cout << "Terminating!" << std::endl;
        }
    }
}