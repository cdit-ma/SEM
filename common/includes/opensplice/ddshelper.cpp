#include "ddshelper.h"
#include <iostream>

ospl::DdsHelper* ospl::DdsHelper::singleton_ = 0;
std::mutex ospl::DdsHelper::global_mutex_;

ospl::DdsHelper* ospl::DdsHelper::get_dds_helper(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_ == 0){
        singleton_ = new DdsHelper();
        std::cout << "Constructed OSPL DDS Helper: " << singleton_ << std::endl;
    }
    return singleton_;
};


dds::domain::DomainParticipant ospl::DdsHelperS::get_participant(int domain){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex);

    //Use the dds find functionality to look for the domain participant for the domain
    auto participant = dds::domain::find(domain);
    if(participant == dds::core::null){
        //No Domain Participant found, so create one.
        //Get Default QOS
        dds::domain::qos::DomainParticipantQos qos;

        participant = dds::domain::DomainParticipant(domain, qos);
        participant.retain();
    }
    return participant;
};

dds::pub::Publisher ospl::DdsHelperS::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex);

    //Construct hash key (Domain|publisher_name)
    std::string key = std::to_string(participant.domain_id()) + "|" + publisher_name;

    //Construct an empty/null publisher
    dds::pub::Publisher publisher(dds::core::null);

    if(!publisher_lookup_.count(key)){
        //None-found, so construct one, and keep it alive
        dds::pub::qos::PublisherQos qos;
        publisher = dds::pub::Publisher(participant, qos);
        publisher.retain();

        //Put it in the hash
        publisher_lookup_[key] = publisher;
    }else{
        //Get the stored publisher
        publisher = publisher_lookup_[key];
    }
    return publisher;
};

dds::sub::Subscriber ospl::DdsHelperS::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex);

    //Construct hash key (Domain|subscriber_name)
    std::string key = std::to_string(participant.domain_id()) + "|" + subscriber_name;

    //Construct an empty/null subscriber
    dds::sub::Subscriber subscriber(dds::core::null);

    if(!subscriber_lookup_.count(key)){
        //None-found, so construct one, and keep it alive
        dds::sub::qos::SubscriberQos qos;
        subscriber = dds::sub::Subscriber(participant, qos);
        subscriber.retain();

        //Put it in the hash
        subscriber[key] = publisher;
    }else{
        //Get the stored publisher
        subscriber = subscriber_lookup_[key];
    }
    return subscriber;
};