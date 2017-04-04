#include "registrant.h"

#include <zmq.hpp>
#include <iostream>
#include <chrono>

zmq::Registrant::Registrant(DeploymentManager* manager, std::string endpoint){
    deployment_manager_ = manager;
    
    //Construct context
    context_ = new zmq::context_t(1);


    
    //Start the registration thread
    registration_thread_ = new std::thread(&zmq::Registrant::RegistrationLoop, this, endpoint);
}

zmq::Registrant::~Registrant(){
    //Deleting the context will interupt any blocking ZMQ calls
    
    std::cout << "Tearing down Context" << std::endl;
    if(context_){
        delete context_;    
    }

    std::cout << "Tearing registration_thread_" << std::endl;
    if(registration_thread_){
        registration_thread_->join();
        delete registration_thread_;
    }

    std::cout << "TeardownModelLogger" << std::endl;

    if(deployment_manager_){
        deployment_manager_->TeardownModelLogger();
        std::cout << "Torndown Model Logger" << std::endl;
        delete deployment_manager_;
    }

    std::cout << "~Registrant" << std::endl;
}

void zmq::Registrant::RegistrationLoop(std::string endpoint){
    //Start a request socket, and bind endpoint
    auto socket = zmq::socket_t(*context_, ZMQ_PAIR);
    socket.bind(endpoint.c_str());

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

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

        std::cout << "------------[Slave Info]------------" << std::endl;
        std::cout << "* Master Endpoint: " << master_control_pub_addr_str << std::endl;
        std::cout << "* Logger Endpoint: " << slave_logging_pub_addr_str << std::endl;
        std::cout << "* Slave Hostname: " << slave_name_str << std::endl;
        std::cout << "------------------------------------" << std::endl;

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

        //Wait until we can guarantee messages
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        zmq::message_t slave_response(reply_message.c_str(), reply_message.size());
        //Send our setup response to the server, blocks until reply
        socket.send(slave_response);

        //zmq::message_t master_response;

         //Get the tcp endpoint for ModelLogger
       // socket.recv(&master_response);

    }catch(const zmq::error_t& exception){
        if(exception.num() == ETERM){
            std::cout << "Terminating!" << std::endl;
        }
        std::cout << "EXCEPTION!" << exception.what() << std::endl;
    }
}