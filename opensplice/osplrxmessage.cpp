#include "osplrxmessage.h"
#include "convert.h"

#include <iostream>

ospl::RxMessage::RxMessage(rxMessageInt* component, dds::sub::Subscriber subscriber, std::string topic_name){
    this->component_ = component;
    this->subscriber_ = subscriber;
     
    //Try find first
    auto topic = dds::topic::find<dds::topic::Topic<test_dds::Message> >(subscriber_.participant(), topic_name);
    if(topic == dds::core::null){
        //Construct
        topic = dds::topic::Topic<test_dds::Message>(subscriber_.participant(), topic_name); 
    }

    reader_ = dds::sub::DataReader<test_dds::Message>(subscriber_, topic);

    rec_thread_ = new std::thread(&RxMessage::recieve, this);
}

void ospl::RxMessage::rxMessage(Message* message){
    component_->rxMessage(message);
}

void ospl::RxMessage::recieve(){
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