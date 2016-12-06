#include "rtitxmessage.h"

#include <rti/rti.hpp>
#include <dds/domain/DomainParticipant.hpp>
#include <dds/sub/Subscriber.hpp>
#include <dds/pub/Publisher.hpp>
#include <dds/sub/DataReader.hpp>
#include <dds/topic/Topic.hpp>


#include "message.hpp"
#include "rtihelper.h"

test_dds::Message rti::translate(::Message *m){
        auto message = test_dds::Message();

        message.time(m->time());
        message.instName(m->instName());
        message.content(m->content());

        return message;
}

rti::TxMessage::TxMessage(txMessageInt* component, int domain_id, std::string publisher_name, std::string writer_name, std::string topic_name){
    this->component_ = component;

    this->domain_id = domain_id;
    this->publisher_name = publisher_name;
    this->writer_name = writer_name;
    this->topic_name = topic_name;
    
    auto participant = rti::get_participant(domain_id);
    auto publisher = rti::get_publisher(participant, publisher_name);
    auto topic = rti::get_topic<test_dds::Message>(participant, topic_name);
    auto writer = rti::get_data_writer<test_dds::Message>(publisher, topic, writer_name);
}

void rti::TxMessage::txMessage(Message* message){

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    

    auto participant = rti::get_participant(domain_id);
    auto publisher = rti::get_publisher(participant, publisher_name);
    auto topic = rti::get_topic<test_dds::Message>(participant, topic_name);
    auto writer = rti::get_data_writer<test_dds::Message>(publisher, topic, writer_name);
    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    
    std::cout << "TOOK: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
    writer.write(translate(message)); 
}