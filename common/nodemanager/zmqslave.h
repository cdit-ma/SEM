#ifndef ZMQSLAVE_H
#define ZMQSLAVE_H

#include <google/protobuf/message_lite.h>
#include "zmq.hpp"

#include <vector>
#include <thread>
#include <string>

class ZMQSlave{
    public:
        ZMQSlave(std::string host_name, std::string port);
        ~ZMQSlave();
    private:
        void registration_loop();
        void action_subscriber_loop();

        std::string master_server_address_;
        std::string host_name_;
        std::string port_;

        std::thread* registration_thread_ = 0;
        std::thread* reader_thread_ = 0;
        zmq::context_t* context_ = 0;
};

#endif //ZMQSLAVE_H