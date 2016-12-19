#ifndef RTI_DDS_HELPER_H
#define RTI_DDS_HELPER_H

#include <iostream>
#include <string>

//Include RTI DDS Headers
#include <rti/rti.hpp>

namespace rti{
    class DdsHelper{
        public:
            static DdsHelper* get_dds_helper();
            dds::domain::DomainParticipant get_participant(int domain);
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);
        
            template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
            template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
            template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
        private:
            dds::core::QosProvider get_qos_provider(std::string qos_uri);
    };
};

inline rti::DdsHelper* rti::DdsHelper::get_dds_helper(){
    //Magic static allocation, only in the scope of this function.
    static rti::DdsHelper* singleton_ = 0;
    if(singleton_ == 0){
        singleton_ = new DdsHelper();
    }
    return singleton_;
};

inline dds::core::QosProvider rti::DdsHelper::get_qos_provider(std::string qos_uri){
    //Construct a new QosProvider using the QOS XML file URI provided
    //TODO: Potentially cache these.
    auto q = dds::core::QosProvider(qos_uri);

    /* //HOW TO LOAD QOS FROM A PROFILE
    try{
        qos = q.participant_qos("LIBRARY:PROFILE");
    }catch(dds::core::Error e){
        std::cerr << "RTI DDS Participant: Domain #" << domain << " Couldn't load Profile: '" << qos_profile << "' From: '" << qos_uri << "'" << std::endl;
    }*/

    q->load_profiles();
    return q;
};

inline dds::domain::DomainParticipant rti::DdsHelper::get_participant(int domain){
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

inline dds::pub::Publisher rti::DdsHelper::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
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

inline dds::sub::Subscriber rti::DdsHelper::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
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

template<class M> dds::topic::Topic<M> rti::DdsHelper::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    //Use the dds find functionality to look for the topic
    auto topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    if(topic == dds::core::null){
        //No Topic found, so create one.
        topic = dds::topic::Topic<M>(participant, topic_name);
        topic.retain();
    }
    return topic;
};

template<class M> dds::pub::DataWriter<M> rti::DdsHelper::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    dds::pub::DataWriter<M> writer = dds::core::null;

    //Construct a writer, using the publisher and topic  
    if(publisher != dds::core::null && topic != dds::core::null){
        writer = dds::pub::DataWriter<M>(publisher, topic);
        writer.retain();
    }

    return writer;
};

template<class M> dds::sub::DataReader<M> rti::DdsHelper::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    dds::sub::DataReader<M> reader = dds::core::null;
    
    //Construct a reader, using the subscriber and topic
    if(subscriber != dds::core::null && topic != dds::core::null){
        reader = dds::sub::DataReader<M>(subscriber, topic);
        reader.enable();
        reader.retain();
    }
    return reader;
};

#endif //RTI_DDS_HELPER_H