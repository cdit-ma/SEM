#ifndef OSPL_OUTEVENTPORT_H
#define OSPL_OUTEVENTPORT_H

#include "../../core/eventports/outeventport.hpp"
#include "../../core/modellogger.h"
#include <string>
#include <mutex>


#include "helper.hpp"

namespace ospl{
     
     template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(Component* component, std::string name);
            void tx(T* message);

            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();

            bool Activate();
            bool Passivate();

        private:
            std::mutex control_mutex_;

            bool configured_ = false;

            std::string topic_name_;
            int domain_id_;
            std::string publisher_name_;
            std::string qos_profile_path_;
            std::string qos_profile_name_;
            dds::pub::DataWriter<S> writer_ = dds::pub::DataWriter<S>(dds::core::null);
    };
};

template <class T, class S>
void ospl::OutEventPort<T, S>::tx(T* message){
    if(this->is_active() && writer_ != dds::core::null){
        auto m = ospl::translate(message);
        //De-reference the message and send
        writer_.write(*m);
        delete m;
        ::OutEventPort<T>::tx(message);
    }
};

template <class T, class S>
ospl::OutEventPort<T, S>::OutEventPort(Component* component, std::string name):
::OutEventPort<T>(component, name, "ospl")
{};


template <class T, class S>
void ospl::OutEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(attributes.count("publisher_name")){
        publisher_name_ = attributes["publisher_name"]->get_String();
        configured_ = true;
    }
    if(attributes.count("topic_name")){
        topic_name_ = attributes["topic_name"]->get_String();
        configured_ = true && configured_;
    }
    if(attributes.count("domain_id")){
        domain_id_ = attributes["domain_id"]->get_Integer();
        configured_ = true && configured_;                
    }
    if(attributes.count("qos_profile_name")){
        qos_profile_name_ = attributes["qos_profile_name"]->get_String();
    }
    if(attributes.count("qos_profile_path")){
        qos_profile_path_ = attributes["qos_profile_path"]->get_String();
    }

    std::cout << "ospl::OutEventPort" << std::endl;
    std::cout << "**domain_id_: "<< domain_id_ << std::endl;
    std::cout << "**publisher_name_: "<< publisher_name_ << std::endl;
    std::cout << "**topic_name_: "<< topic_name_ << std::endl << std::endl;
    std::cout << "**qos_profile_path: " << qos_profile_path_ << std::endl;
    std::cout << "**qos_profile_name: " << qos_profile_name_ << std::endl << std::endl;
};

template <class T, class S>
bool ospl::OutEventPort<T, S>::Teardown(){
    Passivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
    return ::OutEventPort<T>::Teardown();
};

template <class T, class S>
bool ospl::OutEventPort<T, S>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);


    //Construct a DDS Participant, Publisher, Topic and Writer
    auto helper = ospl::DdsHelper::get_dds_helper();
    auto participant = helper->get_participant(domain_id_);
    auto publisher = helper->get_publisher(participant, publisher_name_);
    auto topic = get_topic<S>(participant, topic_name_);
    writer_ = get_data_writer<S>(publisher, topic);

    return ::OutEventPort<T>::Activate();
};

template <class T, class S>
bool ospl::OutEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(writer_ != dds::core::null){
        writer_ = dds::pub::DataWriter<S>(dds::core::null);
    }

    return ::OutEventPort<T>::Passivate();
};

#endif //OSPL_OUTEVENTPORT_H