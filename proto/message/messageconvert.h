#ifndef PROTO_MESSAGE_CONVERT_H
#define PROTO_MESSAGE_CONVERT_H


#include "../../message.h"

namespace proto{
    class Message;

    proto::Message* translate(::Message* message);
    ::Message* translate(proto::Message* message);
    //Special helpers

    ::Message* decode(std::string message, proto::Message* m);
    std::string encode(::Message* message);
};

#endif //PROTO_MESSAGE_CONVERT_H
