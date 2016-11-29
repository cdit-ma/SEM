#ifndef ZMQ_CONVERT_H
#define ZMQ_CONVERT_H

#include "message.pb.h"
#include "../message.h"
namespace proto{
proto::Message* message_to_proto(Message* message){
    proto::Message* out = new proto::Message();
    out->set_time(message->time());
    out->set_instname(message->instName());
    out->set_content(message->content());
    return out;
}

Message* proto_to_message(proto::Message* message){
    Message* out = new Message();
    out->set_instName(message->instname());
    out->set_time(message->time());
    out->set_content(message->content());
    return out;
}
}

#endif // ZMQ_CONVERT_H
