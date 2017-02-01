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
    class DdsHelper{
        public:
            
            dds::domain::DomainParticipant get_participant(int domain);
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);
        
            template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
            template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
            template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
        private:
            //Lookups for OpenSplice
            std::unordered_map<std::string, dds::pub::Publisher> publisher_lookup_;
            std::unordered_map<std::string, dds::sub::Subscriber> subscriber_lookup_;
            std::mutex mutex_;
    };
    
    //Define a global mutex 
    static std::mutex global_mutex_;
    static ospl::DdsHelper* singleton_ = 0;
    ospl::DdsHelper* get_dds_helper();
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

inline dds::pub::Publisher ospl::DdsHelper::get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name){
    std::lock_guard<std::mutex> lock(mutex_);
    //Construct hash key (Domain|publisher_name)
    std::string key = std::to_string(participant.domain_id()) + "|" + publisher_name;

    //Construct an empty/null publisher
    dds::pub::Publisher publisher(dds::core::null);

    //Search pub_lookup_ for key
    auto search = publisher_lookup_.find(key);
    
    if(search == publisher_lookup_.end()){
        //None-found, so construct one, and keep it alive
        dds::pub::qos::PublisherQos qos;
        publisher = dds::pub::Publisher(participant, qos);
        publisher.retain();

        //construct a pair to insert
        std::pair<std::string, dds::pub::Publisher> insert_pair(key, publisher);
        //Insert into hash
        publisher_lookup_.insert(insert_pair);
    }else{
        //Use the stored publisher
        publisher = search->second;
    }
    return publisher;
};

inline dds::sub::Subscriber ospl::DdsHelper::get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name){
    std::lock_guard<std::mutex> lock(mutex_);
    //Construct hash key (Domain|subscriber_name)
    std::string key = std::to_string(participant.domain_id()) + "|" + subscriber_name;

    //Construct an empty/null subscriber
    dds::sub::Subscriber subscriber(dds::core::null);

    //Search subscriber_lookup_ for key
    auto search = subscriber_lookup_.find(key);
    
    if(search == subscriber_lookup_.end()){
        //None-found, so construct one, and keep it alive
        dds::sub::qos::SubscriberQos qos;
        subscriber = dds::sub::Subscriber(participant, qos);
        subscriber.retain();

        //construct a pair to insert
        std::pair<std::string, dds::sub::Subscriber> insert_pair(key, subscriber);
        //Insert into hash
        subscriber_lookup_.insert(insert_pair);
    }else{
        //Use the stored subscriber
        subscriber = search->second;
    }
    return subscriber;
};

template<class M> dds::topic::Topic<M> ospl::DdsHelper::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    std::lock_guard<std::mutex> lock(mutex_);
    //Use the dds find functionality to look for the topic
    auto topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    if(topic == dds::core::null){
        //No Topic found, so create one.
        topic = dds::topic::Topic<M>(participant, topic_name);
        topic.retain(); 
    }
    return topic;
};

template<class M> dds::pub::DataWriter<M> ospl::DdsHelper::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    std::lock_guard<std::mutex> lock(mutex_);
    dds::pub::DataWriter<M> writer = dds::core::null;
    
    if(publisher != dds::core::null && topic != dds::core::null){
        writer = dds::pub::DataWriter<M>(publisher, topic);
        writer.retain();
    }

    return writer;
};

template<class M> dds::sub::DataReader<M> ospl::DdsHelper::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    std::lock_guard<std::mutex> lock(mutex_);
    //Construct an empty/null reader
    dds::sub::DataReader<M> reader(dds::core::null);
    
    if(subscriber != dds::core::null && topic != dds::core::null){
        reader = dds::sub::DataReader<M>(subscriber, topic);
        reader.retain();
    }
    return reader;
};

#endif //OSPL_DDS_HELPER_H