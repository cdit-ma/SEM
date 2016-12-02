#include "convert.h"
#include <iostream>
test_dds::Message message_to_dds(::Message* message){
        test_dds::Message out;
        
        out.time(message->time());
        out.instName(message->instName());
        out.content(message->content());

        return out;
}

::Message* dds_to_message(test_dds::Message message){
        ::Message* out = new ::Message();
        out->set_instName(message.instName());
        out->set_time(message.time());
        out->set_content(message.content());
        return out;
    }