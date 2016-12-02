#include "osplrxmessage.h"
#include "convert.h"

#include <iostream>
#include "message.h"
#include "message_DCPS.hpp"

ospl_rxMessage::ospl_rxMessage(rxMessageInt* component, dds::sub::Subscriber subscriber, std::string topic_name)
    /*:subscriber_(dds::core::null),
    reader_(dds::core::null)*/
{
    this->component_ = component;
    this->subscriber_ = subscriber;

    dds::topic::Topic<test_dds::Message> topic(subscriber_.participant(), topic_name);

    reader_ = dds::sub::DataReader<test_dds::Message>(subscriber_, topic);

    
    rec_thread_ = new std::thread(&ospl_rxMessage::recieve, this);
}

void ospl_rxMessage::rxMessage(Message* message){
    component_->rxMessage(message);
}

void ospl_rxMessage::recieve(){
    while(true){ 
        auto samples = reader_.take();

        for(auto sample_it = samples.begin(); sample_it != samples.end(); ++sample_it){
            if(sample_it->info().valid()){
                Message* m = opensplice::opensplice_to_message(sample_it->data());
                rxMessage(m);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}