#include "osplrxmessage.h"

#include <iostream>

#include <dds/dds.hpp>
#include "message_DCPS.hpp"
#include "osplhelper.h"


::Message* ospl::translate(ospl::Message m){
    auto message = new ::Message();
    message->set_instName(m.instName());
    message->set_time(m.time());
    message->set_content(m.content());
    return message;
}

ospl::RxMessage::RxMessage(rxMessageInt* component, int domain_id, std::string subscriber_name,std::string reader_name, std::string  topic_name){
    this->component_ = component;

    this->domain_id = domain_id;
    this->subscriber_name = subscriber_name;
    this->reader_name = reader_name;
    this->topic_name = topic_name;
    


    auto participant = ospl::get_participant(domain_id);
    auto subscriber = ospl::get_subscriber(participant, this->subscriber_name);
    auto topic = ospl::get_topic<ospl::Message>(participant, this->topic_name);
   
    rec_thread_ = new std::thread(&RxMessage::recieve, this);
}

void ospl::RxMessage::rxMessage(::Message* message){
    component_->rxMessage(message);
}

void ospl::RxMessage::recieve(){
    
    auto participant = ospl::get_participant(domain_id);
    auto subscriber = ospl::get_subscriber(participant, subscriber_name);
    auto topic = ospl::get_topic<ospl::Message>(participant, topic_name);
    auto reader = ospl::get_data_reader<ospl::Message>(subscriber,topic, reader_name);
    while(true){ 
        auto samples = reader.take();

        for(auto sample_it = samples.begin(); sample_it != samples.end(); ++sample_it){
            if(sample_it->info().valid()){
                
                ::Message* m = ospl::translate(sample_it->data());
                rxMessage(m);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}