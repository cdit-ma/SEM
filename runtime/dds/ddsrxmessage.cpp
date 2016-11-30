#include "ddsrxmessage.h"
#include "convert.h"


#include <iostream>

dds_rxMessage::dds_rxMessage(rxMessageInt* component, DDSSubscriber* subscriber, DDSTopic* topic){
    this->component_ = component;
    this->subscriber_ = subscriber;
    this->topic_ = topic;
    
    rec_thread_ = new std::thread(&dds_rxMessage::recieve, this);
}

void dds_rxMessage::rxMessage(Message* message){
    component_->rxMessage(message);
}

void dds_rxMessage::recieve(){
    DDS_DataReaderQos reader_qos;
    subscriber_->get_participant()->get_default_datareader_qos(reader_qos);

    reader_ = (test_dds::MessageDataReader* ) subscriber_->create_datareader(topic_,
                                                        reader_qos, 
                                                        NULL,
                                                        DDS_STATUS_MASK_ALL);
    while(true){
        //https://community.rti.com/kb/how-implement-generic-read-and-write-methods-using-cpp-templates
        DDS_SampleInfo info;
        
        //Construct Data
        test_dds::Message* out = test_dds::Message::TypeSupport::create_data();

        //Take the data.        
        DDS_ReturnCode_t result = reader_->take_next_sample(*out, info);
        if (result == DDS_RETCODE_OK && info.valid_data) {
            //if we have valid data, use it.
            Message* m = dds_to_message(out);
            rxMessage(m);

        }
        //Clean up
        test_dds::Message::TypeSupport::delete_data(out);
         std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

}