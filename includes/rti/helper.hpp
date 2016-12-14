#ifndef RTI_DDS_HELPER_H
#define RTI_DDS_HELPER_H

#include <iostream>
#include <string>

#include <rti/rti.hpp>

namespace rti{
    class DdsHelper{
        public:
            static DdsHelper* get_dds_helper(){
                static rti::DdsHelper* singleton_ = 0;
                if(singleton_ == 0){
                    singleton_ = new DdsHelper();
                }
                return singleton_;
            };
            dds::domain::DomainParticipant get_participant(int domain){
                dds::domain::DomainParticipant p = dds::domain::find(domain);
                if(p == dds::core::null){
                    dds::domain::qos::DomainParticipantQos qos;
                    //Forces RTI to not use Shared Memory
                    qos->transport_builtin.mask(rti::core::policy::TransportBuiltinMask::udpv4());
                    p = dds::domain::DomainParticipant(domain, qos);
                    p.retain();
                }
                return p;
            };
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
                dds::pub::Publisher pub = rti::pub::find_publisher(participant, publisher_name);
                if(pub == dds::core::null){
                    dds::pub::qos::PublisherQos qos;
                    qos << rti::core::policy::EntityName(publisher_name);
                    pub = dds::pub::Publisher(participant, qos);
                    pub.retain();
                }
                return pub;
            };
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
                dds::sub::Subscriber sub = rti::sub::find_subscriber(participant, subscriber_name);
                if(sub == dds::core::null){
                    dds::sub::qos::SubscriberQos qos;
                    qos << rti::core::policy::EntityName(subscriber_name);
                    sub = dds::sub::Subscriber(participant,qos);
                    sub.retain();
                }
                return sub;
            };
            template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
            template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic);
            template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic);
    };
};

template<class M> dds::topic::Topic<M> rti::DdsHelper::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    auto topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    if(topic == dds::core::null){
        //Construct
        topic = dds::topic::Topic<M>(participant, topic_name);
        topic.enable();
        topic.retain();
    }
    return topic;
};

template<class M> dds::pub::DataWriter<M> rti::DdsHelper::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic){
     dds::pub::DataWriter<M> writer = dds::core::null;
    
    if(publisher != dds::core::null && topic != dds::core::null){
        writer = dds::pub::DataWriter<M>(publisher, topic);
        writer.enable();
        writer.retain();
    }

    return writer;

};

template<class M> dds::sub::DataReader<M> rti::DdsHelper::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic){
    //Construct an empty/null reader
    dds::sub::DataReader<M> reader(dds::core::null);
    
    if(subscriber != dds::core::null && topic != dds::core::null){
        reader = dds::sub::DataReader<M>(subscriber, topic);
        reader.enable();
        reader.retain();
    }
    return reader;
};



#endif //RTI_DDS_HELPER_H