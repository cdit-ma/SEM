#ifndef ZMQ_CONVERT_H
#define ZMQ_CONVERT_H

#include "message.pb.h"
#include "../message.h"

namespace proto{
    proto::Message* message_to_proto(::Message* message);
    ::Message* proto_to_message(proto::Message* message);
    
};

#endif // ZMQ_CONVERT_H
