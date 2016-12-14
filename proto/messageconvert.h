#ifndef PROTO_MESSAGE_CONVERT_H
#define PROTO_MESSAGE_CONVERT_H

#include "message.pb.h"
#include "../message.h"

namespace proto{
    proto::Message* translate(::Message* message);
    ::Message* translate(proto::Message* message);
    //Special helpers

    ::Message* decode(std::string message);
    std::string encode(::Message* message);
};

#endif //PROTO_MESSAGE_CONVERT_H
