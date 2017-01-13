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
        void reader_loop();

        std::string master_server_address_;
        std::string host_name_;
        std::string port_;
    private:
        std::thread* registration_thread_;
        std::thread* reader_thread_;
        zmq::context_t* context_;
};

#endif //ZMQSLAVE_H