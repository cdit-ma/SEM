#ifndef RTI_DDS_HELPER_H
#define RTI_DDS_HELPER_H

#include <iostream>
#include <string>

#include <rti/rti.hpp>


namespace rti{

    class DdsHelper{
        public:
            static DdsHelper* get_dds_helper();
            dds::domain::DomainParticipant get_participant(int domain);
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);
            template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
            template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string reader_name);
            template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string writer_name);

        private:
            static DdsHelper* singleton_;
    };
};

template<class M> dds::topic::Topic<M> rti::DdsHelper::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    auto topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    if(topic == dds::core::null){
        std::cout << "RTI Construcing Topic: " << topic_name << " For Domain: " << participant.domain_id() << std::endl; 
        //Construct
        topic = dds::topic::Topic<M>(participant, topic_name); 
    }
    return topic;
};

template<class M> dds::pub::DataWriter<M> rti::DdsHelper::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string writer_name){
    dds::pub::DataWriter<M> writer = rti::pub::find_datawriter_by_topic_name<dds::pub::DataWriter<M> >(publisher, writer_name);
    if(writer == dds::core::null){
        std::cout << "RTI Constructing DataWriter: " << writer_name << std::endl;
        
        writer = dds::pub::DataWriter<M>(publisher, topic);
        //qos << rti::core::policy::EntityName(writer_name);
        //writer.qos(qos);
        writer.retain();
    }
    return writer;
};

template<class M> dds::sub::DataReader<M> rti::DdsHelper::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string reader_name){
    dds::sub::DataReader<M> reader = rti::sub::find_datareader_by_name<dds::sub::DataReader<M> >(subscriber, reader_name);
    if(reader == dds::core::null){
        std::cout << "RTI Constructing DataReader: " << reader_name << std::endl;
        dds::sub::qos::DataReaderQos qos;
        qos << rti::core::policy::EntityName(reader_name);
        reader = dds::sub::DataReader<M>(subscriber, topic, qos);
        reader.retain();
    }
    return reader;
};
};

rti::RtiHelper* rti::RtiHelper::singleton_ = 0;

rti::RtiHelper* rti::RtiHelper::get_dds_helper(){
    if(singleton_ == 0){
        singleton_ = new RtiHelper();
    }
    return singleton_;
};

dds::domain::DomainParticipant rti::RtiHelper::get_participant(int domain){
    dds::domain::DomainParticipant p = dds::domain::find(domain);
    if(p == dds::core::null){
        dds::domain::qos::DomainParticipantQos qos;
        qos->transport_builtin.mask(rti::core::policy::TransportBuiltinMask::udpv4());
        std::cout << "RTI CONSTRUCTING: " << domain << std::endl;
        p = dds::domain::DomainParticipant(domain, qos);
        p.retain();
    }
    return p;
};

dds::pub::Publisher rti::RtiHelper::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    dds::pub::Publisher pub = rti::pub::find_publisher(participant, publisher_name);
    if(pub == dds::core::null){
        std::cout << "RTI CONSTRUCTING PUB: " << publisher_name << std::endl;                
        dds::pub::qos::PublisherQos qos;
        qos << rti::core::policy::EntityName(publisher_name);

        pub = dds::pub::Publisher(participant, qos);

        pub.retain();
    }

    return pub;
};

dds::sub::Subscriber rti::RtiHelper::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    dds::sub::Subscriber sub = rti::sub::find_subscriber(participant, subscriber_name);
    if(sub == dds::core::null){
        std::cout << "RTI CONSTRUCTING SUB: " << subscriber_name << std::endl;        
        dds::sub::qos::SubscriberQos qos;
        qos << rti::core::policy::EntityName(subscriber_name);
        sub = dds::sub::Subscriber(participant,qos);

        sub.retain();
    }
    return sub;
};

#endif //RTIHELPER_H