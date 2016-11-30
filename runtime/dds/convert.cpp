#include "convert.h"

test_dds::Message* message_to_dds(::Message* message){
        test_dds::Message* out = test_dds::Message::TypeSupport::create_data();

        out->time = message->time();
        strncpy(out->instName, message->instName().c_str(), message->instName().length()+1);
        strncpy(out->content, message->content().c_str(), message->content().length()+1);

        return out;
}

::Message* dds_to_message(test_dds::Message* message){
        ::Message* out = new ::Message();
        out->set_instName(message->instName);
        out->set_time(message->time);
        out->set_content(message->content);
        return out;
    }