#include "osplhelper.h"
#include <iostream>


ospl::OsplHelper* ospl::OsplHelper::singleton_ = 0;


ospl::OsplHelper* ospl::OsplHelper::get_dds_helper(){
    if(singleton_ == 0){
        singleton_ = new OsplHelper();
    }
    return singleton_;
}


dds::domain::DomainParticipant ospl::OsplHelper::get_participant(int domain){
    //Use the dds find functionality to look for the domain participant
    dds::domain::DomainParticipant participant = dds::domain::find(domain);
    if(participant == dds::core::null){
        //None-found, so construct one, and keep it alive
        dds::domain::qos::DomainParticipantQos qos;
        participant = dds::domain::DomainParticipant(domain, qos);
        participant.retain();
    }
    return participant;
}

dds::pub::Publisher ospl::OsplHelper::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    //Construct hash key (Domain|publisher_name)
    std::string key = publisher_name;
    //std::string key = std::to_string(participant.domain_id()) + "|" + publisher_name;

    //Construct an empty/null publisher
    dds::pub::Publisher publisher(dds::core::null);

    //Search pub_lookup_ for key
    auto search = publisher_lookup_.find(key);
    
    if(search == publisher_lookup_.end()){
        //None-found, so construct one, and keep it alive
        publisher = dds::pub::Publisher(participant);
        publisher.retain();

        //construct a pair to insert
        std::pair<std::string, dds::pub::Publisher> insert_pair(publisher_name, publisher);
        //Insert into hash
        publisher_lookup_.insert(insert_pair);
    }else{
        //Use the stored publisher
        publisher = search->second;
    }
    return publisher;
}

dds::sub::Subscriber ospl::OsplHelper::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    //Construct hash key (Domain|subscriber_name)
    std::string key = std::to_string(participant.domain_id()) + "|" + subscriber_name;

    //Construct an empty/null subscriber
    dds::sub::Subscriber subscriber(dds::core::null);

    //Search subscriber_lookup_ for key
    auto search = subscriber_lookup_.find(key);
    
    if(search == subscriber_lookup_.end()){
        //None-found, so construct one, and keep it alive
        subscriber = dds::sub::Subscriber(participant);
        subscriber.retain();

        //construct a pair to insert
        std::pair<std::string, dds::sub::Subscriber> insert_pair(subscriber_name, subscriber);
        //Insert into hash
        subscriber_lookup_.insert(insert_pair);
    }else{
        //Use the stored subscriber
        subscriber = search->second;
    }
    return subscriber;
}



