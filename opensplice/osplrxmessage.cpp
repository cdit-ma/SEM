#include "osplrxmessage.h"
#include "osplhelper.h"
#include "message_DCPS.hpp"

ospl::RxMessage::RxMessage(rxMessageInt* component, int domain_id, std::string subscriber_name,std::string reader_name, std::string  topic_name){
    this->component_ = component;

    //Get the opensplice helper
    auto helper = ospl::OsplHelper::get_ospl_helper();

    //Construct/get the domain participant, subscriber, topic and reader
    auto participant = helper->get_participant(domain_id);
    auto subscriber = helper->get_subscriber(participant, subscriber_name);
    auto topic = helper->get_topic<ospl::Message>(participant, topic_name);
    auto reader = helper->get_data_reader<ospl::Message>(subscriber,topic, reader_name);

    //Set our local writer
    reader_ = new dds::sub::AnyDataReader(reader);

    rec_thread_ = new std::thread(&RxMessage::recieve, this);
}

void ospl::RxMessage::rxMessage(::Message* message){
    component_->rxMessage(message);
}

void ospl::RxMessage::recieve(){
    //Get our typed reader
    auto reader = reader_->get<ospl::Message>();

    while(true){         
        auto samples = reader.take();

        for(auto sample_it = samples.begin(); sample_it != samples.end(); ++sample_it){
            //Recieve our valid samples
            if(sample_it->info().valid()){
                auto m = translate(&sample_it->data());
                rxMessage(m);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}