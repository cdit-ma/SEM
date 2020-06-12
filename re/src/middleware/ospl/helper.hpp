#ifndef OSPL_DDS_HELPER_H
#define OSPL_DDS_HELPER_H

#include <iostream>
#include <string>
#include <mutex>

//Include OpenSplice DDS Headers
#include <dds/dds.hpp>

#include "ddshelper.h"

namespace ospl{
    template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
    template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
    template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
};

template<class M> dds::topic::Topic<M> ospl::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    auto lock = get_dds_helper().obtain_lock();

    dds::topic::Topic<M> topic(dds::core::null);
    try{
        //Try use the Middleware's find function to find the topic
        topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    }catch(dds::core::InvalidDowncastError	e){
        std::cerr << "ospl::get_topic: Error: " << e.what() << std::endl;
    }

    if(topic == dds::core::null){
        //If we can't find the topic, we should construct it
        topic = dds::topic::Topic<M>(participant, topic_name);
        topic.retain();
        //std::cerr << "ospl::get_topic: Constructed Topic: " << topic_name << std::endl;
    }
    return topic;
};

template<class M> dds::pub::DataWriter<M> ospl::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    auto lock = get_dds_helper().obtain_lock();

    dds::pub::DataWriter<M> writer(dds::core::null);
    
    //If we have the publisher and the topic, construct the writer.
    if(publisher != dds::core::null && topic != dds::core::null){
        dds::pub::qos::DataWriterQos qos;
        if(qos_uri != ""){
            std::cout << "Setting QOS on OSPL not currently supported." << std::endl;
            //dds::core::QosProvider qos_provider(qos_uri);
            //qos = qos_provider.datawriter_qos();
        }

        writer = dds::pub::DataWriter<M>(publisher, topic, qos);
        writer.retain();
        //std::cout << "ospl::get_data_writer: Constructed DataWriter" << std::endl;
    }
    return writer;
};

template<class M> dds::sub::DataReader<M> ospl::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    auto lock = get_dds_helper().obtain_lock();
    
    dds::sub::DataReader<M> reader(dds::core::null);
    
    //If we have the subscriber and the topic, construct the writer.
    if(subscriber != dds::core::null && topic != dds::core::null){
        dds::sub::qos::DataReaderQos qos;
        if(qos_uri != ""){
            std::cout << "Setting QOS on OSPL not currently supported." << std::endl;
            //dds::core::QosProvider qos_provider(qos_uri);
            //qos = qos_provider.datareader_qos();
        }

        reader = dds::sub::DataReader<M>(subscriber, topic, qos);
        reader.retain();
        //std::cout << "ospl::get_data_reader: Constructed DataReader" << std::endl;
    }
    return reader;
};

#endif //OSPL_DDS_HELPER_H