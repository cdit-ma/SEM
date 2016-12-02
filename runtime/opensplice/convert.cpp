#include "convert.h"

test_dds::Message opensplice::message_to_opensplice(::Message* message){
        test_dds::Message out;
        //WTF
        DDS::Long t = message->time();
        out.time(t);

        out.instName(message->instName());
        out.content(message->content());
        return out;
}

::Message* opensplice::opensplice_to_message(test_dds::Message message){
        ::Message* out = new ::Message();
        out->set_instName(message.instName());
        out->set_time(message.time());
        out->set_content(message.content());
        return out;
}