#include "convert.h"
#include "ddstxmessage.h"

dds_txMessage::dds_txMessage(txMessageInt* component, DDSPublisher* publisher){
    this->component_ = componenet;
    this->publisher_ = publisher;

    DDS_DataReaderQos writerQos;

     writer_ = publisher_->create_datawriter(topic, writerQos, NULL, DDS_STATUS_MASK_ALL);



}


void dds_txMessage::txMessage(Message* message){
    test_dds::Message* m = message_to_dds(message);
    
    
    writer_->write(m, DDS_HANDLE_NIL);

}