#include "convert.h"

ospl::Message* opensplice::message_to_opensplice(::Message* message){
        ospl::Message* out = new ospl::Message();
        out->time = message->time();
        out->instName = message->instName().c_str();
        out->content = message->content().c_str();
        return out;
}

::Message* opensplice::opensplice_to_message(ospl::Message* message){
        ::Message* out = new ::Message();
        out->set_instName(DDS::string_dup(message->instName));
        out->set_time(message->time);
        out->set_content(DDS::string_dup(message->content));
        return out;
}