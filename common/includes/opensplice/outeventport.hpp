#ifndef OSPL_OUTEVENTPORT_H
#define OSPL_OUTEVENTPORT_H

#include "../core/eventports/outeventport.hpp"

#include <string>

#include "helper.hpp"

namespace ospl{
     template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(Component* component, int domain_id, std::string publisher_name, std::string topic_name);
            void tx(T* message);
        private:
            dds::pub::DataWriter<S> writer_ = dds::pub::DataWriter<S>(dds::core::null);
    }; 
};

template <class T, class S>
void ospl::OutEventPort<T, S>::tx(T* message){
    if(writer_ != dds::core::null){
        auto m = translate(message);
        //De-reference the message and send
        writer_.write(*m);
        delete m;
    }else{
        //No writer
    }
};

template <class T, class S>
ospl::OutEventPort<T, S>::OutEventPort(Component* component, int domain_id, std::string publisher_name, std::string topic_name){
    //Construct a DDS Participant, Publisher, Topic and Writer
    auto helper = DdsHelper::get_dds_helper();   
    auto participant = helper->get_participant(domain_id);
    auto publisher = helper->get_publisher(participant, publisher_name);
    auto topic = helper->get_topic<S>(participant, topic_name);
    writer_ = helper->get_data_writer<S>(publisher, topic);
};

#endif //OSPL_OUTEVENTPORT_H