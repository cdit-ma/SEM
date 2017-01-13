#include "zmqslave.h"
#include <iostream>
#include <chrono>

ZMQSlave::ZMQSlave(std::string host_name, std::string port){
    context_ = new zmq::context_t(1);
    host_name_ = host_name;
    port_ = port;
    registration_thread_ = new std::thread(&ZMQSlave::registration_loop, this);
    
}

ZMQSlave::~ZMQSlave(){

}


void ZMQSlave::registration_loop(){
    auto socket = zmq::socket_t(*context_, ZMQ_REQ);
    std::cout << "Slave Binding: " << port_ << std::endl;
    socket.bind(port_.c_str());
    


    
    zmq::message_t *message = new zmq::message_t();
    zmq::message_t* d2 = new zmq::message_t();
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << " Waiting for Master!" << std::endl;
        try{
            zmq::message_t data = zmq::message_t(host_name_.c_str(), host_name_.size());
            
            socket.send(data, ZMQ_NOBLOCK);
            socket.recv(d2);
            
            std::string msg_str(static_cast<char *>(d2->data()), d2->size());

            

            std::cout << "Connected to Serverino: " << msg_str << std::endl;
            break;
        }catch(...){
            //std::cout << "NO RECIEVIER: " << std::endl;
            
        }
    }
}