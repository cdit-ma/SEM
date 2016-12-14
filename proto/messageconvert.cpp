#include "messageconvert.h"

proto::Message* proto::translate(::Message* message){
        proto::Message* out = new proto::Message();
        out->set_time(message->time());
        out->set_instname(message->instName());
        out->set_content(message->content());
        return out;
}

::Message* proto::translate(proto::Message* message){
        ::Message* out = new ::Message();
        out->set_instName(message->instname());
        out->set_time(message->time());
        out->set_content(message->content());
        return out;
}

::Message* proto::decode(std::string message){
        auto pb = new proto::Message();
        pb->ParseFromString(message);
        auto m = translate(pb);
        delete pb;
        return m;
}

std::string proto::encode(::Message* message){
        std::string str;
        auto pb = translate(message);
        pb->SerializeToString(&str);
        delete pb;
        return str;
}