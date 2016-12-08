#include "rtirxmessage.h"


#include <rti/rti.hpp>
#include <dds/domain/DomainParticipant.hpp>
#include <dds/sub/Subscriber.hpp>
#include <dds/pub/Publisher.hpp>
#include <dds/sub/DataReader.hpp>
#include <dds/topic/Topic.hpp>

#include "message.hpp"


#include "rtihelper.h"

::Message* rti::translate(rti::Message m){
    auto message = new ::Message();
    message->set_instName(m.instName());
    message->set_time(m.time());
    message->set_content(m.content());
    return message;
}

rti::RxMessage::RxMessage(rxMessageInt* component, int domain_id, std::string subscriber_name, std::string reader_name, std::string topic_name){
    this->component_ = component;

    this->domain_id = domain_id;
    this->subscriber_name = subscriber_name;
    this->reader_name = reader_name;
    this->topic_name = topic_name;
    

    auto helper = rti::RtiHelper::get_rti_helper();
    auto participant = helper->get_participant(domain_id);
    auto subscriber = helper->get_subscriber(participant, subscriber_name);
    auto topic = helper->get_topic<rti::Message>(participant, topic_name);
   
    rec_thread_ = new std::thread(&RxMessage::recieve, this);
}

void rti::RxMessage::rxMessage(::Message* message){
    component_->rxMessage(message);
}

void rti::RxMessage::recieve(){
    
    auto helper = rti::RtiHelper::get_rti_helper();
    auto participant = helper->get_participant(domain_id);
    auto subscriber = helper->get_subscriber(participant, subscriber_name);
    auto topic = helper->get_topic<rti::Message>(participant, topic_name);
    auto reader = helper->get_data_reader<rti::Message>(subscriber,topic, reader_name);
    while(true){ 
        auto samples = reader.take();

        for(auto sample_it = samples.begin(); sample_it != samples.end(); ++sample_it){
            if(sample_it->info().valid()){
                ::Message* m = rti::translate(sample_it->data());
                rxMessage(m);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}