#include "ddshelper.h"
#include <iostream>

ospl::Helper& ospl::get_dds_helper(){
    static ospl::Helper helper_;
    return helper_;
};


dds::domain::DomainParticipant ospl::Helper::get_participant(int domain){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex_);

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

dds::pub::Publisher ospl::Helper::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex_);

    //Construct hash key (Domain|publisher_name)
    std::string key = std::to_string(participant.domain_id()) + "|" + publisher_name;

    //Construct an empty/null publisher
    dds::pub::Publisher publisher(dds::core::null);

    //Search pub_lookup_ for key
    auto search = publisher_lookup_.find(key);

    if(search == publisher_lookup_.end()){
        //None-found, so construct one, and keep it alive
        dds::pub::qos::PublisherQos qos;
        publisher = dds::pub::Publisher(participant, qos);
        publisher.retain();

        //construct a pair to insert
        std::pair<std::string, dds::pub::Publisher> insert_pair(key, publisher);
        //Insert into hash
        publisher_lookup_.insert(insert_pair);
    }else{
        //Get the stored publisher
        publisher = search->second;
    }
    return publisher;
};

dds::sub::Subscriber ospl::Helper::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex_);

    //Construct hash key (Domain|subscriber_name)
    std::string key = std::to_string(participant.domain_id()) + "|" + subscriber_name;

    //Construct an empty/null subscriber
    dds::sub::Subscriber subscriber(dds::core::null);
    
    //Search pub_lookup_ for key
    auto search = subscriber_lookup_.find(key);

    if(search == subscriber_lookup_.end()){
        //None-found, so construct one, and keep it alive
        dds::sub::qos::SubscriberQos qos;
        subscriber = dds::sub::Subscriber(participant, qos);
        subscriber.retain();

        //construct a pair to insert
        std::pair<std::string, dds::sub::Subscriber> insert_pair(key, subscriber);
        //Insert into hash
        subscriber_lookup_.insert(insert_pair);
    }else{
        //Get the stored subscriber
        subscriber = search->second;
    }
    return subscriber;
};

std::unique_lock<std::mutex> ospl::Helper::obtain_lock(){
    std::unique_lock<std::mutex> lock(mutex_);
    return std::move(lock);
}