#include "ddstxmessage.h"
#include "convert.h"

#include <iostream>

dds_txMessage::dds_txMessage(txMessageInt* component, dds::pub::Publisher* publisher, dds::topic::Topic<test_dds::Message>* topic){
    this->component_ = component;
    this->publisher_ = publisher;

    writer_ = new dds::pub::DataWriter<test_dds::Message>(*publisher_, *topic);
}


void dds_txMessage::txMessage(Message* message){
    test_dds::Message m = message_to_dds(message);
    std::cout << "Writing: " << m << std::endl;
    writer_->write(m);
}