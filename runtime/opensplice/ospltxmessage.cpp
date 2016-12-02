#include "ospltxmessage.h"
#include "convert.h"

#include <iostream>

ospl_txMessage::ospl_txMessage(txMessageInt* component, dds::pub::Publisher publisher, std::string topic_name)
:publisher_(dds::core::null),
writer_(dds::core::null)
{
    this->component_ = component;
    this->publisher_ = publisher;

    dds::topic::Topic<test_dds::Message> topic(publisher_.participant(), topic_name);

    //construct a DDS writer
    writer_ = dds::pub::DataWriter<test_dds::Message>(publisher_, topic);
}

void ospl_txMessage::txMessage(Message* message){
    test_dds::Message m = opensplice::message_to_opensplice(message);
    writer_.write(m);
}
