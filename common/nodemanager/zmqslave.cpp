#include "zmqslave.h"
#include <iostream>
#include <chrono>



ZMQSlave::ZMQSlave(std::string host_name, std::string port){
    context_ = new zmq::context_t(1);
    host_name_ = host_name;
    port_ = port;

    //Start the registration thread
    registration_thread_ = new std::thread(&ZMQSlave::registration_loop, this);
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

    if(reader_thread_){
        reader_thread_->join();
        delete reader_thread_;
    }
}

void ZMQSlave::registration_loop(){
    auto socket = zmq::socket_t(*context_, ZMQ_REQ);
    socket.bind(port_.c_str());

    //Wait for a period of time before trying to send
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //Construct a message to send to the server
    zmq::message_t slave_addr(port_.c_str(), port_.size());
    zmq::message_t server_addr;

    try{
        //Send our address to the server, blocks until reply
        socket.send(slave_addr);
        //Get the tcp address for the action publisher.
        socket.recv(&server_addr);

        //Cast the ZMQ response to a std::string
        master_server_address_ = std::string(static_cast<char *>(server_addr.data()), server_addr.size());
        
        std::cout << "Got Action Publisher End-Point: " << master_server_address_ << std::endl;

        //Start the action subscriber loop
        reader_thread_ = new std::thread(&ZMQSlave::action_subscriber_loop, this);
    }catch(const zmq::error_t& exception){
        if(exception.num() == ETERM){
            std::cout << "Terminating!" << std::endl;
        }
    }
}

void ZMQSlave::action_subscriber_loop(){

    //Construct a filter for this hostname
    //Add a * char so we don't get messages for other similar hosts.
    //gouda1 would match gouda11 but gouda1* doesn't match gouda11*
    std::string name = host_name_ + "*";

    //Construct a Subscriber socket and connect
    auto socket = zmq::socket_t(*context_, ZMQ_SUB);
    socket.connect(master_server_address_.c_str());

    //Filter the global messages
    socket.setsockopt(ZMQ_SUBSCRIBE, "*", 1);

    //Filter the specific messages for this node
    socket.setsockopt(ZMQ_SUBSCRIBE, name.c_str(), name.size());

    //Wait for a period of time before recieving
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    zmq::message_t node;
    zmq::message_t action;

    while(true){
        try{
            //Recieve the action's target node first
            socket.recv(&node);
            //Recieve the action second
            socket.recv(&action);

            //Get the messages as strings
            std::string node_str(static_cast<char *>(node.data()), node.size());
            std::string action_str(static_cast<char *>(action.data()), action.size());
            std::cout << "Action For:" << node_str <<" A:" << action_str <<std::endl;
        }catch(const zmq::error_t& exception){
            if(exception.num() == ETERM){
                std::cout << "Terminating!" << std::endl;
                //Caught exception
                break;
            }
        }
    }
}