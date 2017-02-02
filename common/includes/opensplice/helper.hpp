#ifndef OSPL_DDS_HELPER_H
#define OSPL_DDS_HELPER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

//Include OpenSplice DDS Headers
#include <dds/dds.hpp>

namespace ospl{
    template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
    template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
    template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
};

inline ospl::DdsHelper* ospl::get_dds_helper(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_ == 0){
        singleton_ = new DdsHelper();
        std::cout << "------------------OSPL SINGLETON------------------" << singleton_ << std::endl;
    }
    return singleton_;
};


inline dds::domain::DomainParticipant ospl::DdsHelper::get_participant(int domain){
    std::lock_guard<std::mutex> lock(mutex_);
    //Use the dds find functionality to look for the domain participant for the domain
    dds::domain::DomainParticipant participant = dds::domain::find(domain);
    if(participant == dds::core::null){
        //No Domain Participant found, so create one.
        //Get Default QOS
        dds::domain::qos::DomainParticipantQos qos;
        participant = dds::domain::DomainParticipant(domain, qos);
        participant.retain();
    }
    return participant;
};


template<class M> dds::topic::Topic<M> ospl::DdsHelper::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    std::lock_guard<std::mutex> lock(DdsHelper::get_dds_helper()->mutex);

    //Use the dds find functionality to look for the topic
    auto topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    if(topic == dds::core::null){
        auto type_name = dds::topic::topic_type_name<M>::value();
        std::cout << "Constructing Topic: " << topic_name << " TYPE NAME: " << type_name<< std::endl;
        //No Topic found, so create one.
        topic = dds::topic::Topic<M>(participant, topic_name, type_name + "_" + topic_name);
        topic.retain();
        std::cout << "Constructed Topic: " << topic_name << std::endl;
    }
    return topic;
};

template<class M> dds::pub::DataWriter<M> ospl::DdsHelper::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    std::lock_guard<std::mutex> lock(DdsHelper::get_dds_helper()->mutex);
    //std::lock_guard<std::mutex> lock(mutex_);
    dds::pub::DataWriter<M> writer = dds::core::null;

    //Construct a writer, using the publisher and topic  
    if(publisher != dds::core::null && topic != dds::core::null){
        writer = dds::pub::DataWriter<M>(publisher, topic);
        writer.retain();
        std::cout << "Constructed DataWriter: " << std::endl;
    }

    return writer;
};

template<class M> dds::sub::DataReader<M> ospl::DdsHelper::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    std::lock_guard<std::mutex> lock(DdsHelper::get_dds_helper()->mutex);
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

#endif //OSPL_DDS_HELPER_H