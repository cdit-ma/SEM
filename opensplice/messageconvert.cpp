#include "messageconvert.h"

#include "message_DCPS.hpp"


ospl::Message* ospl::translate(::Message *m){
        auto message = new ospl::Message();
        int t = m->time();
        message->time(t);
        message->instName(m->instName());
        message->content(m->content());
        return message;
}

::Message* ospl::translate(const ospl::Message *m){
        auto message = new ::Message();
        int t = m->time();
        message->set_time(t);
        message->set_instName(m->instName());
        message->set_content(m->content());
        return message;
}
