#include "registrar.h"
#include <iostream>
#include <zmq.hpp>
#include "../executionmanager.h"

zmq::Registrar::Registrar(ExecutionManager* manager, std::string publisher_endpoint){
    //Construct a context for creating sockets
    context_ = new zmq::context_t();

    //Set the publisher_endpoint which we will send to the Registrants
    publisher_endpoint_ = publisher_endpoint;
    
    //Set our execution manager    
    execution_manager_ = manager;

    //Construct a new thread for each slave
    for(auto s : execution_manager_->GetRequiredSlaveEndpoints()){
        auto t = new std::thread(&zmq::Registrar::RegistrationLoop, this, s);
    }
}

zmq::Registrar::~Registrar(){
    //Deleting the context will interupt any blocking ZMQ calls
    if(context_){
        delete context_;    
    }

    //Join all registration threads
    while(!registration_threads_.empty()){
        auto t = registration_threads_.back();
        if(t){
            t->join();
            delete t;
        }
        registration_threads_.pop_back();
    }

    if(execution_manager_){
        delete execution_manager_;
    }
}

void zmq::Registrar::RegistrationLoop(std::string endpoint){
    //Construct a socket (Using Pair)
    auto socket = zmq::socket_t(*context_, ZMQ_PAIR);

    try{
        //Connect to the socket
        socket.connect(endpoint.c_str()); 
    }catch(zmq::error_t &ex){
        std::cout << "ZMQMaster::RegistrationLoop():Connect(" << endpoint << "): " << ex.what() << std::endl;
    }

    //Messages from the slave
    zmq::message_t slave_addr;
    zmq::message_t slave_response;

    while(true){
        try{
            //Wait for Slave to send its endpoint
            socket.recv(&slave_addr);

            std::string slave_addr_str(static_cast<char *>(slave_addr.data()), slave_addr.size());
            
            //Get the matching hostname from the execution manager
            std::string host_name = execution_manager_->GetHostNameFromAddress(slave_addr_str);
            std::string slave_logger_pub_addr_str = execution_manager_->GetLoggerAddressFromHostName(host_name);

            //Construct our reply messages
            zmq::message_t master_control_pub_addr(publisher_endpoint_.c_str(), publisher_endpoint_.size());
            zmq::message_t slave_name(host_name.c_str(), host_name.size());
            zmq::message_t slave_logging_pub_addr(slave_logger_pub_addr_str.c_str(), slave_logger_pub_addr_str.size());
            
            //Send the server address for the publisher
            socket.send(master_control_pub_addr, ZMQ_SNDMORE);
            //Send the slave hostname
            socket.send(slave_name, ZMQ_SNDMORE);
            //Send the slave hostname
            socket.send(slave_logging_pub_addr);

            //Wait for Slave to send a message
            socket.recv(&slave_response);
            std::string slave_response_str(static_cast<char *>(slave_response.data()), slave_response.size());
            
            //Tell the execution manager
            execution_manager_->SlaveOnline(slave_response_str, slave_addr_str, host_name);
        }catch(const zmq::error_t& exception){
            if(exception.num() != ETERM){
                std::cout << "ZMQMaster::RegistrationLoop(): " << exception.what() << std::endl;
            }
            break;
        }
    }
}