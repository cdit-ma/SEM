#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H

#include <google/protobuf/message_lite.h>
#include "zmq.hpp"

class ZMQMessageWriter{
    public:
        ZMQMessageWriter();
        ~ZMQMessageWriter();

        bool bind_publisher_socket(std::string endpoint);

        virtual bool push_message(google::protobuf::MessageLite* message);
        virtual bool terminate();
    private:
        zmq::context_t* context;
        zmq::socket_t* socket;
};

#endif //MESSAGEWRITER_H