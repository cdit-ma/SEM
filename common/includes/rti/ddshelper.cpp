#include "ddshelper.h"
#include <iostream>
#include <thread>
#include "ndds/ndds_cpp.h"

rti::DdsHelper* rti::DdsHelper::singleton_ = 0;
std::mutex rti::DdsHelper::global_mutex_;

rti::DdsHelper* rti::DdsHelper::get_dds_helper(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_ == 0){
        singleton_ = new DdsHelper();
        NDDSConfigLogger::get_instance()->set_verbosity(NDDS_CONFIG_LOG_VERBOSITY_SILENT );

        std::cout << "Constructed DDS Helper: " << singleton_ << std::endl;
    }
    return singleton_;
};


dds::domain::DomainParticipant rti::DdsHelper::get_participant(int domain){
    std::lock_guard<std::mutex> lock(mutex);
    //Use the dds find functionality to look for the domain participant for the domain
    dds::domain::DomainParticipant participant = dds::domain::find(domain);
    if(participant == dds::core::null){
        //No Domain Participant found, so create one.
        //Get Default QOS
        dds::domain::qos::DomainParticipantQos qos;

        //Forces RTI to not use Shared Memory
        qos->transport_builtin.mask(rti::core::policy::TransportBuiltinMask::udpv4());
        participant = dds::domain::DomainParticipant(domain, qos);
        participant.enable();
        participant.retain();
    }
    return participant;
};

dds::pub::Publisher rti::DdsHelper::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    std::lock_guard<std::mutex> lock(mutex);
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

dds::sub::Subscriber rti::DdsHelper::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    std::lock_guard<std::mutex> lock(mutex);
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