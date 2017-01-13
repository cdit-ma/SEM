#include "zmqmaster.h"
#include <iostream>

ZMQMaster::ZMQMaster(std::string host_name, std::string port, std::vector<std::string> slaves){
    context_ = new zmq::context_t(1);
    port_ = port;
    
    slaves_ = slaves;
    //action_socket_ = new zmq::socket_t(*context_, ZMQ_PUB);
    registration_thread_ = new std::thread(&ZMQMaster::registration_loop, this);
}

ZMQMaster::~ZMQMaster(){

}

bool ZMQMaster::connected_to_slaves(){

    return true;
}

void ZMQMaster::registration_loop(){
    auto socket = zmq::socket_t(*context_, ZMQ_REP);
    //socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);



    //Connect registration socket to all slaves addressess.
    for(auto s : slaves_){
        std::cout << "Master Connecting: " << s << std::endl;
        socket.connect(s.c_str()); 
    }
    int slave_count = slaves_.size();
    int count = 0;
    bool got_all_slaves = false;
    zmq::message_t *data = new zmq::message_t();
    zmq::message_t *reply = new zmq::message_t();

    while(true){
        std::cout << "Slaves: " << count << "/" << slave_count << std::endl;
        //Wait for next message
        socket.recv(data);

        //If we have a valid message
        if(data->size() > 0){
            //Construct a string out of the zmq data
            std::string msg_str(static_cast<char *>(data->data()), data->size());
         
            //Check the hostname
            std::cout << "Got Slave: " << msg_str << std::endl;

            zmq::message_t rep(port_.c_str(), port_.size());
            socket.send(rep);

            count++;         
            
            if(count == slave_count){
                std::cout << " Got Slaves: " << count << std::endl;
                break;
            }
        }
    }
}