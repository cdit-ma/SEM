#include "osplhelper.h"
#include <iostream>


dds::domain::DomainParticipant ospl::get_participant(int domain){
    dds::domain::DomainParticipant p = dds::domain::find(domain);
    if(p == dds::core::null){
        std::cout << "Constructing Domain Participant : " << domain << std::endl;
        dds::domain::qos::DomainParticipantQos qos;
        p = dds::domain::DomainParticipant(domain, qos);
        p.retain();
    }
    return p;
}

dds::pub::Publisher ospl::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    dds::pub::Publisher pub = dds::core::null;//ospl::pub::find_publisher(participant, publisher_name);
    if(pub == dds::core::null){
        std::cout << "Construcing Publisher: " << publisher_name << " For Domain: " << participant.domain_id() << std::endl; 
        //dds::pub::qos::PublisherQos qos = participant.default_publisher_qos() << dds::core::policy::Partition(publisher_name);
        pub = dds::pub::Publisher(participant);//, qos);

        pub.retain();
    }

    return pub;
}

dds::sub::Subscriber ospl::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    dds::sub::Subscriber sub = dds::core::null;// ospl::sub::find_subscriber(participant, subscriber_name);
    if(sub == dds::core::null){
        std::cout << "Construcing Subscriber: " << subscriber_name << " For Domain: " << participant.domain_id() << std::endl;
        //dds::sub::qos::SubscriberQos qos = participant.default_subscriber_qos() << dds::core::policy::Partition(subscriber_name);
        sub = dds::sub::Subscriber(participant);//, qos);

        sub.retain();
    }
    return sub;
}

