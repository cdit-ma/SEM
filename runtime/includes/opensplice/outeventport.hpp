#ifndef OSPL_OUTEVENTPORT_H
#define OSPL_OUTEVENTPORT_H

#include "../globalinterfaces.h"

#include <iostream>
#include <string>

#include "helper.hpp"

namespace ospl{
     template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(::OutEventPort<T>* port, int domain_id, std::string publisher_name, std::string topic_name);
            void notify();
            void tx_(T* message);
        private:
            dds::pub::DataWriter<S> writer_ = dds::pub::DataWriter<S>(dds::core::null);
            ::OutEventPort<T>* port_;
    }; 
};

template <class T, class S>
void ospl::OutEventPort<T, S>::tx_(T* message){
    auto m = translate(message);
    if(writer_ != dds::core::null){
        std::cout << "Sending!" << std::endl;
        //De-reference the message and send
        writer_.write(*m);
    }
    delete m;
};

template <class T, class S>
ospl::OutEventPort<T, S>::OutEventPort(::OutEventPort<T>* port, int domain_id, std::string publisher_name, std::string topic_name){
    this->port_ = port;
    
    auto helper = DdsHelper::get_dds_helper();   
    auto participant = helper->get_participant(domain_id);
    auto publisher = helper->get_publisher(participant, publisher_name);
    auto topic = helper->get_topic<S>(participant, topic_name);
    writer_ = helper->get_data_writer<S>(publisher, topic);
};

#endif //OSPL_OUTEVENTPORT_H
