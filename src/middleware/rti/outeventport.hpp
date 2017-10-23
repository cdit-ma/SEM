#ifndef RTI_OUTEVENTPORT_H
#define RTI_OUTEVENTPORT_H

#include "../../core/eventports/outeventport.hpp"
#include "../../core/modellogger.h"
#include <string>
#include <mutex>


#include "helper.hpp"

namespace rti{
     template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(Component* component, std::string name);
            ~OutEventPort(){
                Passivate();
                Teardown();
            };
            
            void tx(T* message);

            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();
            bool Passivate();
        private:
            void setup_tx();

            std::mutex control_mutex_;

            std::string topic_name_;
            int domain_id_;
            std::string publisher_name_;
            std::string qos_profile_path_;
            std::string qos_profile_name_;

            dds::pub::DataWriter<S> writer_ = dds::pub::DataWriter<S>(dds::core::null);
    }; 
};

template <class T, class S>
void rti::OutEventPort<T, S>::tx(T* message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(this->is_active() && writer_ != dds::core::null){
        auto m = rti::translate(message);
        //De-reference the message and send
        writer_.write(*m);
        delete m;
        ::OutEventPort<T>::tx(message);
    }
};

template <class T, class S>
rti::OutEventPort<T, S>::OutEventPort(Component* component, std::string name):
::OutEventPort<T>(component, name, "rti"){};

template <class T, class S>
void rti::OutEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    bool configured_ = false;
    {
        std::lock_guard<std::mutex> lock(control_mutex_);

        if(attributes.count("publisher_name")){
            publisher_name_ = attributes["publisher_name"]->get_String();
        }
        if(attributes.count("topic_name")){
            topic_name_ = attributes["topic_name"]->get_String();
        }
        if(attributes.count("domain_id")){
            domain_id_ = attributes["domain_id"]->get_Integer();
        }

        
        if(attributes.count("qos_profile_name")){
            qos_profile_name_ = attributes["qos_profile_name"]->get_String();
        }
        if(attributes.count("qos_profile_path")){
            qos_profile_path_ = attributes["qos_profile_path"]->get_String();
        }
        

        configured_ = topic_name_.length() && publisher_name_.length() && domain_id_ >= 0;
    }

    if(configured_){
        setup_tx();
    }else{
        std::cerr << "rti::OutEventPort<T, S>(" << this->get_id() << " " << this->get_name() << ")::Startup: Not correcly configured!" << std::endl;
        std::cerr << "\t*Domain ID: "<< domain_id_ << std::endl;
        std::cerr << "\t*Publisher Name: "<< publisher_name_ << std::endl;
        std::cerr << "\t*Topic Name: "<< topic_name_ << std::endl;
        std::cerr << "\t*QOS Profile Path: " << qos_profile_path_ << std::endl;
        std::cerr << "\t*QOS Profile Name: " << qos_profile_name_ << std::endl << std::endl;
    }
};

template <class T, class S>
bool rti::OutEventPort<T, S>::Teardown(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::Teardown()){
        return true;
    }
    return false;
};

template <class T, class S>
void rti::OutEventPort<T, S>::setup_tx(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Construct a DDS Participant, Publisher, Topic and Writer
    auto helper = DdsHelper::get_dds_helper();   
    auto participant = helper->get_participant(domain_id_);
    auto topic = get_topic<S>(participant, topic_name_);
    
    //std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    auto publisher = helper->get_publisher(participant, publisher_name_);
    writer_ = get_data_writer<S>(publisher, topic, qos_profile_path_, qos_profile_name_);
    
};

template <class T, class S>
bool rti::OutEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(writer_ != dds::core::null){
        writer_ = dds::core::null;
    }

    return ::OutEventPort<T>::Passivate();
};

#endif //RTI_OUTEVENTPORT_H
