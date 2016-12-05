#include "rtitxmessage.h"
#include "convert.h"

#include <iostream>

rti::TxMessage::TxMessage(txMessageInt* component, dds::pub::Publisher publisher, std::string topic_name){
    this->component_ = component;
    this->publisher_ = publisher;

    //Try find first
    auto topic = dds::topic::find<dds::topic::Topic<test_dds::Message> >(publisher_.participant(), topic_name);
    if(topic == dds::core::null){
        //Construct
        topic = dds::topic::Topic<test_dds::Message>(publisher_.participant(), topic_name); 
    }

    //construct a DDS writer
    writer_ = dds::pub::DataWriter<test_dds::Message>(publisher_, topic);
}


void rti::TxMessage::txMessage(Message* message){
    test_dds::Message m = message_to_dds(message);
    writer_.write(m);
}