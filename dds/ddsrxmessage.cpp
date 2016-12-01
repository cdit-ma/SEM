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
        auto samples = reader_->take();

        for(auto sample_it = samples.begin(); sample_it != samples.end(); ++sample_it){
            if(sample_it->info().valid()){
                Message* m = dds_to_message(sample_it->data());
                rxMessage(m);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}