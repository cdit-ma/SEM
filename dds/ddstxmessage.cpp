#include "convert.h"
#include <iostream>
#include "ddstxmessage.h"

dds_txMessage::dds_txMessage(txMessageInt* component, DDSPublisher* publisher, DDSTopic* topic){
    this->component_ = component;
    this->publisher_ = publisher;

    DDS_DataWriterQos writerQos;

    publisher->get_participant()->get_default_datawriter_qos(writerQos);

    //Construct DataWriter
    writer_ = (test_dds::MessageDataWriter*) publisher_->create_datawriter(topic, writerQos, NULL, DDS_STATUS_MASK_ALL);



}


void dds_txMessage::txMessage(Message* message){
    test_dds::Message* m = message_to_dds(message);
    
    writer_->write(*m, DDS_HANDLE_NIL);
    std::cout << "WRITING MESSAGE: " << message->time() << std::endl;
}