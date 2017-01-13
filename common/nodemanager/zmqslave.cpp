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
            
            


            master_server_address_ = std::string(static_cast<char *>(d2->data()), d2->size());
            std::cout << "Connected to Serverino: " << master_server_address_ << std::endl;
            break;
        }catch(...){
            //std::cout << "NO RECIEVIER: " << std::endl;
        }
    }
    std::cout << "WAITING FOR READER" << std::endl;
    reader_thread_ = new std::thread(&ZMQSlave::reader_loop, this);
}

void ZMQSlave::reader_loop(){
    std::string name = host_name_ + "*";
    auto socket = zmq::socket_t(*context_, ZMQ_SUB);
    socket.connect(master_server_address_.c_str());
    socket.setsockopt(ZMQ_SUBSCRIBE, "*", 1);
    socket.setsockopt(ZMQ_SUBSCRIBE, name.c_str(), name.size());
    
    

    zmq::message_t *topic = new zmq::message_t();
    zmq::message_t *data = new zmq::message_t();

    while(true){
        socket.recv(topic);
        socket.recv(data);

        std::string topic_str(static_cast<char *>(topic->data()), topic->size());
        std::string data_str(static_cast<char *>(data->data()), data->size());
        std::cout << "Action For:" << topic_str <<" A:" << data_str <<std::endl;
    }
    std::cout << "action_loop thread Finished." << std::endl;
}