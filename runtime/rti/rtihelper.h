#ifndef RTIHELPER_H
#define RTIHELPER_H

#include <iostream>
#include <string>
#include <rti/rti.hpp>

namespace rti{
    dds::domain::DomainParticipant get_participant(int domain);

    dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);    
    dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);





template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    auto topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    if(topic == dds::core::null){
        std::cout << "RTI Construcing Topic: " << topic_name << " For Domain: " << participant.domain_id() << std::endl; 
        //Construct
        topic = dds::topic::Topic<M>(participant, topic_name); 
    }
    return topic;
};

template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string writer_name){
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

template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string reader_name){
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
#endif //RTIHELPER_H