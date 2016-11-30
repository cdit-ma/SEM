#include "convert.h"
#include <iostream>
test_dds::Message* message_to_dds(::Message* message){
        test_dds::Message* out = test_dds::Message::TypeSupport::create_data();
        out->time = message->time();
        out->instName = DDS_String_dup(message->instName().c_str());
        out->content = DDS_String_dup(message->content().c_str());
        return out;
}

::Message* dds_to_message(test_dds::Message* message){
        ::Message* out = new ::Message();
        out->set_instName(message->instName);
        out->set_time(message->time);
        out->set_content(message->content);
        return out;
    }