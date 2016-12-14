#include "osplrxmessage.h"

#include "message_DCPS.hpp"

//Include the templated OutEventPort Implementation for OSPL
#include "opensplice/ineventport.hpp"



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
}

void ospl::RxMessage::rxMessage(::Message* message){
    component_->rxMessage(message);
     //Construct a concrete Ospl InEventPort linked to callback into this.
    this->event_port_ = new ospl::InEventPort<::Message, cdit::Message>(this, domain_id, subscriber_name, reader_name, topic_name);
}

void ospl::RxMessage::rxMessage(::Message* message){
    //Call back into the component.
    component_->rxMessage(message);
}

void ospl::RxMessage::rx_(::Message* message){
    //Call back into the component.
    rxMessage(message);
}