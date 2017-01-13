#ifndef ZMQMASTER_H
#define ZMQMASTER_H

#include <google/protobuf/message_lite.h>
#include "zmq.hpp"

#include <vector>
#include <thread>
#include <string>

class ZMQMaster{
    public:
        ZMQMaster(std::string host_name, std::string port, std::vector<std::string> slaves);
        ~ZMQMaster();

        bool send_action(std::string node_name, google::protobuf::MessageLite* message){return true;};
        
        bool connected_to_slaves();

    private:
        void registration_loop();

        std::string host_name;
        std::string port_;
        std::vector<std::string> slaves_;
    
    private:
        std::thread* registration_thread_;
        zmq::context_t* context_;
};

#endif //ZMQMASTER_H