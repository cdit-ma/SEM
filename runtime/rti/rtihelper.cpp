#include "rtihelper.h"
#include <iostream>


dds::domain::DomainParticipant rti::get_participant(int domain){
    dds::domain::DomainParticipant p = dds::domain::find(domain);
    if(p == dds::core::null){
        dds::domain::qos::DomainParticipantQos qos;
        qos->transport_builtin.mask(rti::core::policy::TransportBuiltinMask::udpv4());
        std::cout << "RTI CONSTRUCTING: " << domain << std::endl;
        p = dds::domain::DomainParticipant(domain, qos);
        p.retain();
    }
    return p;
}

dds::pub::Publisher rti::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    dds::pub::Publisher pub = rti::pub::find_publisher(participant, publisher_name);
    if(pub == dds::core::null){
        std::cout << "RTI CONSTRUCTING PUB: " << publisher_name << std::endl;                
        dds::pub::qos::PublisherQos qos;
        qos << rti::core::policy::EntityName(publisher_name);

        pub = dds::pub::Publisher(participant, qos);

        pub.retain();
    }

    return pub;
}

dds::sub::Subscriber rti::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    dds::sub::Subscriber sub = rti::sub::find_subscriber(participant, subscriber_name);
    if(sub == dds::core::null){
        std::cout << "RTI CONSTRUCTING SUB: " << subscriber_name << std::endl;        
        dds::sub::qos::SubscriberQos qos;
        qos << rti::core::policy::EntityName(subscriber_name);
        sub = dds::sub::Subscriber(participant,qos);

        sub.retain();
    }
    return sub;
}

