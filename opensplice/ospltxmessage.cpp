#include "ospltxmessage.h"
#include <chrono>
#include "osplhelper.h"


#include "message_DCPS.hpp"

ospl::TxMessage::TxMessage(txMessageInt* component, int domain_id, std::string  publisher_name, std::string  writer_name, std::string  topic_name){
    this->component_ = component;

    auto helper = ospl::OsplHelper::get_ospl_helper();

    auto participant = helper->get_participant(domain_id);
    auto publisher = helper->get_publisher(participant, publisher_name);
    auto topic = helper->get_topic<ospl::Message>(participant, topic_name);
    auto writer = helper->get_data_writer<ospl::Message>(publisher, topic, writer_name);
    
    //Set our writer
    writer_ = new dds::pub::AnyDataWriter(writer);
}

void ospl::TxMessage::txMessage(::Message* message){
    auto writer = writer_->get<ospl::Message>();
    auto m = translate(message);
    writer.write(*m);
    delete m; 
}
