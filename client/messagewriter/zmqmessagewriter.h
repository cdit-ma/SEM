#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H

#include <google/protobuf/message_lite.h>
#include "zmq.hpp"

class ZMQMessageWriter{
    public:
        ZMQMessageWriter();
        virtual ~ZMQMessageWriter();

        bool BindPublisherSocket(std::string endpoint);

        virtual bool PushMessage(google::protobuf::MessageLite* message);
        virtual bool PushString(std::string* message);
        virtual bool Terminate();
    private:
        zmq::socket_t* socket_;
        zmq::context_t* context_;
};

#endif //MESSAGEWRITER_H