#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H

#include <google/protobuf/message_lite.h>
#include "zmq.hpp"

class ZMQMessageWriter{
    public:
        ZMQMessageWriter();
        virtual ~ZMQMessageWriter();

        bool bind_publisher_socket(std::string endpoint);

        virtual bool push_message(google::protobuf::MessageLite* message);
        virtual bool terminate();
    private:
        zmq::socket_t* socket;
        zmq::context_t* context;
};

#endif //MESSAGEWRITER_H