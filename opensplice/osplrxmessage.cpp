#include "osplrxmessage.h"

#include <mutex>

#include "ospldatareaderlistener.h"
#include "osplhelper.h"
#include "message_DCPS.hpp"



ospl::RxMessage::RxMessage(rxMessageInt* component, int domain_id, std::string subscriber_name,std::string reader_name, std::string  topic_name){
    this->component_ = component;

    //Get the dds helper
    auto helper = OsplHelper::get_dds_helper();

    //Construct/get the domain participant, subscriber, topic and reader
    auto participant = helper->get_participant(domain_id);
    auto subscriber = helper->get_subscriber(participant, subscriber_name);
    auto topic = helper->get_topic<ospl::Message>(participant, topic_name);
    auto reader = helper->get_data_reader<ospl::Message>(subscriber,topic, reader_name);

    //Set our local writer_
    auto listener_ = new DataReaderListener<ospl::Message>(this);

    //Only listen to data-available
    reader.listener(listener_, dds::core::status::StatusMask::data_available());
    reader_ = new dds::sub::AnyDataReader(reader);   

    //Setup Thread
    rec_thread_ = new std::thread(&RxMessage::recieve_loop, this);
}4

void ospl::RxMessage::rxMessage(::Message* message){
    component_->rxMessage(message);
}

void ospl::RxMessage::notify(){
    //Notify thread
    std::unique_lock<std::mutex> lock(notify_mutex_);
    notify_lock_condition_.notify_all();
}

void ospl::RxMessage::recieve_loop(){
    //Get our typed reader
    auto reader = reader_->get<ospl::Message>();
    
    while(true){
        {
            //Wait for next message
            std::unique_lock<std::mutex> lock(notify_mutex_);
            notify_lock_condition_.wait(lock);
        }

        auto samples = reader.take();
        for(auto sample_it = samples.begin(); sample_it != samples.end(); ++sample_it){
            //Recieve our valid samples
            if(sample_it->info().valid()){
                auto m = translate(&sample_it->data());
                rxMessage(m);
            }
        }
    }
}