#include "ospltxmessage.h"
#include <chrono>



#include "message_DCPS.hpp"
#include <dds/dds.hpp>

#include "osplhelper.h"

test_dds::Message ospl::translate(::Message *m){
        auto message = test_dds::Message();
        int t = m->time();
        message.time(t);
        //message.time(m->time());
        message.instName(m->instName());
        message.content(m->content());

        return message;
}

ospl::TxMessage::TxMessage(txMessageInt* component, int domain_id, std::string  publisher_name, std::string  writer_name, std::string  topic_name){
    this->component_ = component;

    this->domain_id = domain_id;
    this->publisher_name = publisher_name;
    this->writer_name = writer_name;
    this->topic_name = topic_name;
    
    auto participant = ospl::get_participant(domain_id);
    auto publisher = ospl::get_publisher(participant, this->publisher_name);
    auto topic = ospl::get_topic<test_dds::Message>(participant, this->topic_name);
    auto writer = ospl::get_data_writer<test_dds::Message>(publisher, topic, this->writer_name);
}

void ospl::TxMessage::txMessage(Message* message){

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    

    auto participant = ospl::get_participant(domain_id);
    auto publisher = ospl::get_publisher(participant, publisher_name);
    auto topic = ospl::get_topic<test_dds::Message>(participant, topic_name);
    auto writer = ospl::get_data_writer<test_dds::Message>(publisher, topic, writer_name);
    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    
    std::cout << "TOOK: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
    writer.write(translate(message)); 
}