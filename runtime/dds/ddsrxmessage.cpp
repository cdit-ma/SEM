#include "ddsrxmessage.h"
#include "convert.h"


#include <iostream>

dds_rxMessage::dds_rxMessage(rxMessageInt* component, dds::sub::Subscriber* subscriber, dds::topic::Topic<test_dds::Message>* topic){
    this->component_ = component;
    this->subscriber_ = subscriber;
    this->topic_ = topic;
    
    rec_thread_ = new std::thread(&dds_rxMessage::recieve, this);
}

void dds_rxMessage::rxMessage(Message* message){
    component_->rxMessage(message);
}

void dds_rxMessage::recieve(){

    reader_ = new dds::sub::DataReader<test_dds::Message>(*subscriber_, *topic_);
    while(true){
        
        //Construct Data
        test_dds::Message* out = new test_dds::Message();

        //Take the data.
        if(reader_->take(*out)){
            Message* m = dds_to_message(out);
            rxMessage(m);

        }
        
        //Clean up
        delete out;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

}