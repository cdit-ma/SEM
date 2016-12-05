#include "rtitxmessage.h"
#include "convert.h"

#include <iostream>

test_dds::Message convert_message(::Message *m){
        test_dds::Message out;

        out.time(m->time());
        out.instName(m->instName());
        out.content(m->content());

        return out;
}

rti::TxMessage::TxMessage(txMessageInt* component, dds::pub::Publisher publisher, std::string topic_name){
    this->component_ = component;
    
    tx_ = new DDS_TX_Interface<test_dds::Message, ::Message>(publisher, topic_name);
}

void rti::TxMessage::txMessage(Message* message){
    tx_->push_message(message);
}