#ifndef OSPLHELPER_H
#define OSPLHELPER_H

#include <iostream>
#include <string>
#include <unordered_map>

#include <dds/dds.hpp>

namespace ospl{
    class OsplHelper{
        public:
            static OsplHelper* get_dds_helper();

            dds::domain::DomainParticipant get_participant(int domain);
            dds::pub::Publisher get_publisher(dds::domain::DomainParticipant participant, std::string publisher_name);    
            dds::sub::Subscriber get_subscriber(dds::domain::DomainParticipant participant, std::string subscriber_name);
            template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
            template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string reader_name);
            template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string writer_name);

        private:
            static OsplHelper* singleton_;
            OsplHelper(){};
            ~OsplHelper(){};
            //Lookups for OpenSplice
            std::unordered_map<std::string, dds::pub::AnyDataWriter> datawriter_lookup_;
            std::unordered_map<std::string, dds::sub::AnyDataReader> datareader_lookup_;
            std::unordered_map<std::string, dds::pub::Publisher> publisher_lookup_;
            std::unordered_map<std::string, dds::sub::Subscriber> subscriber_lookup_;
    };
};


template<class M> dds::topic::Topic<M> ospl::OsplHelper::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    //Use the standard dds find to look for the topic object for this participant
    auto topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    if(topic == dds::core::null){
        //Construct the topic if we haven't seen it
        topic = dds::topic::Topic<M>(participant, topic_name);
        topic.retain(); 
    }
    return topic;
};

template<class M> dds::pub::DataWriter<M> ospl::OsplHelper::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string writer_name){
    //Construct hash key (Domain|type_name|topic_name|writer_name)
    std::string key = writer_name;
    //std::string key = std::to_string(publisher.participant().domain_id()) + "|" + topic.type_name() + "|" + topic.name() + "|" + writer_name;

    //Construct an empty/null writer
    dds::pub::DataWriter<M> writer(dds::core::null);

    //Search datawriter_lookup_ for key
    auto search = datawriter_lookup_.find(key);
    
    if(search == datawriter_lookup_.end()){
        //None-found, so construct one, and keep it alive
        writer = dds::pub::DataWriter<M>(publisher, topic);
        writer.retain();
        
        //store as a type-less DataReader
        dds::pub::AnyDataWriter w(writer);
        
        //construct a pair to insert
        std::pair<std::string, dds::pub::AnyDataWriter> insert_pair(key, w);
        
        //Insert into hash
        datawriter_lookup_.insert(insert_pair);
    }else{
        //Get the type-Less DataWriter and get original type back
        dds::pub::AnyDataWriter w = search->second;
        writer = w.get<M>();
    }
    return writer;
};

template<class M> dds::sub::DataReader<M> ospl::OsplHelper::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string reader_name){
    //Construct hash key (Domain|type_name|topic_name|writer_name)
    std::string key = std::to_string(subscriber.participant().domain_id()) + "|" + topic.type_name() + "|" + topic.name() + "|" + reader_name;

    //Construct an empty/null reader
    dds::sub::DataReader<M> reader(dds::core::null);

    //Search datareader_lookup_ for key
    auto search = datareader_lookup_.find(key);
    
    if(search == datareader_lookup_.end()){
        //None-found, so construct one, and keep it alive
        reader = dds::sub::DataReader<M>(subscriber, topic);
        reader.retain();
        
        //store as a type-less DataReader
        dds::sub::AnyDataReader r(reader);
        
        //construct a pair to insert
        std::pair<std::string, dds::sub::AnyDataReader> insert_pair(key, r);
        
        //Insert into hash
        datareader_lookup_.insert(insert_pair);
    }else{
        //Get the type-Less DataReader and get original type back
        dds::sub::AnyDataReader r = search->second;
        reader = r.get<M>();
    }
    return reader;
};

#endif //OSPLHELPER_H