#include "ddshelper.h"

#include <iostream>

rti::DdsHelperS* rti::DdsHelperS::singleton_ = 0;
std::mutex rti::DdsHelperS::global_mutex_;

rti::DdsHelperS* rti::DdsHelperS::get_dds_helper(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_ == 0){
        singleton_ = new DdsHelperS();
        std::cout << "Constructed DDS Helper: " << singleton_ << std::endl;
    }
    std::cout << "Got DDS Helper: " << singleton_ << std::endl;
    return singleton_;
};


dds::domain::DomainParticipant rti::DdsHelperS::get_participant(int domain){
    std::lock_guard<std::mutex> lock(mutex_);
    //Use the dds find functionality to look for the domain participant for the domain
    dds::domain::DomainParticipant participant = dds::domain::find(domain);
    if(participant == dds::core::null){
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

dds::pub::Publisher rti::DdsHelperS::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    std::lock_guard<std::mutex> lock(mutex_);
    //Use the dds find functionality to look for the publisher on that domain
    dds::pub::Publisher pub = rti::pub::find_publisher(participant, publisher_name);
    if(pub == dds::core::null){
        //No Publisher found, so create one.
        //Get Default QOS
        dds::pub::qos::PublisherQos qos;
        //Set the publisher name
        qos << rti::core::policy::EntityName(publisher_name);
        pub = dds::pub::Publisher(participant, qos);
        pub.retain();
    }
    return pub;
};

dds::sub::Subscriber rti::DdsHelperS::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    std::lock_guard<std::mutex> lock(mutex_);
    //Use the dds find functionality to look for the subscriber on that domain
    dds::sub::Subscriber sub = rti::sub::find_subscriber(participant, subscriber_name);
    if(sub == dds::core::null){
        //No Subscriber found, so create one.
        //Get Default QOS
        dds::sub::qos::SubscriberQos qos;
        //Set the subscriber name
        qos << rti::core::policy::EntityName(subscriber_name);
        sub = dds::sub::Subscriber(participant,qos);
        sub.retain();
    }
    return sub;
};