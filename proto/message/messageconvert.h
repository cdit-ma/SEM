#ifndef PROTO_MESSAGE_CONVERT_H
#define PROTO_MESSAGE_CONVERT_H


#include "../../message.h"

namespace proto{
    class Message;

    proto::Message* translate(::Message* message);
    ::Message* translate(proto::Message* message);
    //Special helpers

    template <class T> ::Message* decode(std::string message);
    std::string encode(::Message* message);

    //Forward Declare this function
    template <> ::Message* decode <proto::Message>(std::string message);
};

#endif //PROTO_MESSAGE_CONVERT_H
