#include "messageconvert.h"

#include "message.hpp"


rti::Message* rti::translate(::Message *m){
        auto message = new rti::Message();
        int t = m->time();
        message->time(t);
        message->instName(m->instName());
        message->content(m->content());
        return message;
}

::Message* rti::translate(const rti::Message *m){
        auto message = new ::Message();
        int t = m->time();
        message->set_time(t);
        message->set_instName(m->instName());
        message->set_content(m->content());
        return message;
}
