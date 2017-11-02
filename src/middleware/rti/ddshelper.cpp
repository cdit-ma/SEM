#include "ddshelper.h"
#include <iostream>

rti::DdsHelper* rti::DdsHelper::singleton_ = 0;
std::mutex rti::DdsHelper::global_mutex_;

rti::DdsHelper* rti::DdsHelper::get_dds_helper(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_ == 0){
        singleton_ = new DdsHelper();
    }
    return singleton_;
};


dds::domain::DomainParticipant rti::DdsHelper::get_participant(int domain){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex);

    //Use the dds find functionality to look for the domain participant for the domain
    auto participant = dds::domain::find(domain);
    if(participant == nullptr){
        //No Domain Participant found, so create one.
        //Get Default QOS
        dds::domain::qos::DomainParticipantQos qos;

        //Forces RTI to not use Shared Memory
        qos->transport_builtin.mask(rti::core::policy::TransportBuiltinMask::udpv4());
        participant = dds::domain::DomainParticipant(domain, qos);
        participant.retain();
    }
    return participant;
};

dds::pub::Publisher rti::DdsHelper::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex);
    
    //Use the dds find functionality to look for the publisher on that domain
    auto pub = rti::pub::find_publisher(participant, publisher_name);
    if(pub == nullptr){
        //Get Default QOS
        dds::pub::qos::PublisherQos qos;

        //Set the publisher name
        qos << rti::core::policy::EntityName(publisher_name);
        pub = dds::pub::Publisher(participant, qos);
        pub.retain();
    }
    return pub;
};

dds::sub::Subscriber rti::DdsHelper::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex);
    
    //Use the dds find functionality to look for the subscriber on that domain
    auto sub = rti::sub::find_subscriber(participant, subscriber_name);
    if(sub == nullptr){
        //Get Default QOS
        dds::sub::qos::SubscriberQos qos;

        //Set the subscriber name
        qos << rti::core::policy::EntityName(subscriber_name);
        sub = dds::sub::Subscriber(participant,qos);
        sub.retain();
    }
    return sub;
};