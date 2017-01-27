#include "convert.h"
#include "message.pb.h"

proto::Message* proto::translate(::Message* message){
        auto out = new proto::Message();
        out->set_time(message->time());
        out->set_instname(message->instName());
        out->set_content(message->content());
        return out;
}

::Message* proto::translate(proto::Message* message){
        auto out = new ::Message();
        out->set_instName(message->instname());
        out->set_time(message->time());
        out->set_content(message->content());
        return out;
}

template <>
::Message* proto::decode<proto::Message>(std::string message){
        proto::Message pb;
        pb.ParseFromString(message);
        return translate(&pb);
}

std::string proto::encode(::Message* message){
        std::string str;
        auto pb = translate(message);
        pb->SerializeToString(&str);
        delete pb;
        return str;
}