#ifndef OSPL_OUTEVENTPORT_H
#define OSPL_OUTEVENTPORT_H

#include "../globalinterfaces.h"

#include <iostream>
#include <string>

#include "osplhelper.h"

namespace ospl{
     template <class T, class S> class Ospl_OutEventPort: public ::OutEventPort<T>{
        public:
            Ospl_OutEventPort(::OutEventPort<T>* port, int domain_id, std::string publisher_name, std::string writer_name, std::string topic_name);
            void notify();
            void tx_(T* message);
        private:
            dds::pub::DataWriter<S> writer_ = dds::pub::DataWriter<S>(dds::core::null);
            ::OutEventPort<T>* port_;
    }; 
};

template <class T, class S>
void ospl::Ospl_OutEventPort<T, S>::tx_(T* message){
    //Call the translate function
    auto m = translate(message);
    //De-reference the message and send
    writer_.write(*m);
    delete m;
};

template <class T, class S>
ospl::Ospl_OutEventPort<T, S>::Ospl_OutEventPort(::OutEventPort<T>* port, int domain_id, std::string publisher_name, std::string writer_name, std::string topic_name){
    this->port_ = port;
    
    auto helper = OsplHelper::get_dds_helper();   
    auto participant = helper->get_participant(domain_id);
    auto publisher = helper->get_publisher(participant, publisher_name);
    auto topic = helper->get_topic<S>(participant, topic_name);
    writer_ = helper->get_data_writer<S>(publisher, topic, writer_name);
};

#endif //OSPL_OUTEVENTPORT_H
