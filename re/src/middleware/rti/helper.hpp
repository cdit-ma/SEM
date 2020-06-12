#ifndef RTI_DDS_HELPER_H
#define RTI_DDS_HELPER_H

#include <iostream>
#include <string>
#include <mutex>

//Include RTI DDS Headers
#include <rti/rti.hpp>

#include "ddshelper.h"

namespace rti
{
    template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
    template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
    template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
};

template<class M> dds::topic::Topic<M> rti::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    auto lock = get_dds_helper().obtain_lock();
    dds::topic::Topic<M> topic = nullptr;
    try{
        //Try use the Middleware's find function to find the topic
        topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    }catch(dds::core::InvalidDowncastError	e){
        std::cerr << "rti::get_topic: Error: " << e.what() << std::endl;
    }

    if(topic == nullptr){
        //If we can't find the topic, we should construct it
        topic = dds::topic::Topic<M>(participant, topic_name);
        topic.retain();
        //std::cerr << "rti::get_topic: Constructed Topic: " << topic_name << std::endl;
    }
    
    return topic;
};

template<class M> dds::pub::DataWriter<M> rti::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    auto lock = get_dds_helper().obtain_lock();
    dds::pub::DataWriter<M> writer = nullptr;

    //If we have the publisher and the topic, construct the writer.
    if(publisher != nullptr && topic != nullptr){
        dds::pub::qos::DataWriterQos qos;
        if(qos_uri != ""){
            std::string profile_name = qos_profile + "::" + qos_profile;
            //std::cout << "Setting QOS Profile from: " << qos_uri << " " << profile_name << std::endl;
            dds::core::QosProvider qos_provider(qos_uri, profile_name);
            qos = qos_provider.datawriter_qos();
        }

        //The next setting is used to force dynamic memory allocation for samples with a serialized size of larger than 32K
        qos.policy<rti::core::policy::Property>().set({"dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size", "32768"});
        writer = dds::pub::DataWriter<M>(publisher, topic, qos);
        writer.retain();
        //std::cout << "rti::get_data_writer: Constructed DataWriter QOS" << std::endl;
    }
    return writer;
};

template<class M> dds::sub::DataReader<M> rti::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    auto lock = get_dds_helper().obtain_lock();
    dds::sub::DataReader<M> reader = nullptr;
    
    //If we have the subscriber and the topic, construct the writer.
    if(subscriber != nullptr && topic != nullptr){
        dds::sub::qos::DataReaderQos qos;

        if(qos_uri != ""){
            std::string profile_name = qos_profile + "::" + qos_profile;
            //std::cout << "Setting QOS Profile from: " << qos_uri << " " << profile_name << std::endl;
            dds::core::QosProvider qos_provider(qos_uri, profile_name);
            qos = qos_provider.datareader_qos();
        }
        //The next setting is used to force dynamic memory allocation for samples with a serialized size of larger than 32K
        qos.policy<rti::core::policy::Property>().set({"dds.data_reader.history.memory_manager.fast_pool.pool_buffer_max_size", "32768"});

        reader = dds::sub::DataReader<M>(subscriber, topic, qos);
        reader.retain();
        //std::cout << "rti::get_data_reader: Constructed DataReader QOS" << std::endl;
    }
    return reader;
};

#endif //RTI_DDS_HELPER_H