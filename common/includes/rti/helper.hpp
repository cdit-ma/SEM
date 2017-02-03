#ifndef RTI_DDS_HELPER_H
#define RTI_DDS_HELPER_H

#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>

//Include RTI DDS Headers
#include <rti/rti.hpp>
#include <thread>

#include <rti/domain/DomainParticipantImpl.hpp>
#include "ddshelper.h"
static std::mutex g_mutex_;
namespace rti
{
    template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
    template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
    template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
};

template<class M> dds::topic::Topic<M> rti::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    std::lock_guard<std::mutex> guard(DdsHelper::get_dds_helper()->mutex);

    auto type_name = dds::topic::topic_type_name<M>::value() + "_" + topic_name;
    std::cout << "rti::get_topic: start " << std::this_thread::get_id() << " Topic: " << topic_name << " Type: " << type_name << std::endl;

    dds::topic::Topic<M> topic = nullptr;
    
    try{
        topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    }catch(dds::core::InvalidDowncastError	e){
        std::cout << "Error: " << e.what() << std::endl;
    }

    
    if(topic == nullptr){
        topic = dds::topic::Topic<M>(participant, topic_name);//, type_name);
        if(participant->is_type_registered(type_name)){
            std::cout << "Topic Type: " << type_name << " has been previously registered!" << std::endl;
        }

        if(topic != nullptr){
            topic.retain();
            std::cout << "Constructed Topic: " << topic_name << std::endl;
        }else{
            std::cout << "Can't construct Topic: " << topic_name << std::endl;

        }
    }
        
    return topic;
};

template<class M> dds::pub::DataWriter<M> rti::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    std::lock_guard<std::mutex> guard(DdsHelper::get_dds_helper()->mutex);

    dds::pub::DataWriter<M> writer = dds::core::null;

    //Construct a writer, using the publisher and topic  
    if(publisher != dds::core::null && topic != dds::core::null){
        writer = dds::pub::DataWriter<M>(publisher, topic);
        writer.retain();
        std::cout << "Constructed DataWriter: " << std::endl;
    }
    return writer;
};

template<class M> dds::sub::DataReader<M> rti::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    std::lock_guard<std::mutex> guard(DdsHelper::get_dds_helper()->mutex);
    //std::lock_guard<std::mutex> lock(mutex_);
    dds::sub::DataReader<M> reader = dds::core::null;
    
    //Construct a reader, using the subscriber and topic
    if(subscriber != dds::core::null && topic != dds::core::null){
        reader = dds::sub::DataReader<M>(subscriber, topic);
        reader.retain();
        std::cout << "Constructed DataReader: " << std::endl;
        
    }
    return reader;
};

#endif //RTI_DDS_HELPER_H