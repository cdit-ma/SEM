#ifndef OSPL_INEVENTPORT_H
#define OSPL_INEVENTPORT_H

#include "../../core/eventports/ineventport.hpp"

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "helper.hpp"
#include "datareaderlistener.hpp"

namespace ospl{
     template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function);
            void notify();

            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();

            bool Activate();
            bool Passivate();
        private:
            void receive_loop();
            
            std::thread* rec_thread_ = 0;

            std::mutex notify_mutex_;
            std::mutex control_mutex_;

            std::condition_variable notify_lock_condition_;

            std::string topic_name_;
            int domain_id_ = 0;
            std::string subscriber_name_;
            
            bool passivate_ = false;
            bool configured_ = false;
    }; 
};


template <class T, class S>
void ospl::InEventPort<T, S>::notify(){
    //Called by the DataReaderListener to notify our InEventPort thread to get new data
    std::unique_lock<std::mutex> lock(notify_mutex_);
    notify_lock_condition_.notify_all();
};

template <class T, class S>
void ospl::InEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(attributes.count("topic_name")){
        topic_name_ = attributes["topic_name"]->get_String();
    }

    if(attributes.count("subscriber_name")){
        subscriber_name_ = attributes["subscriber_name"]->get_String();
    }else{
        subscriber_name_ = "In_" + this->get_name();
    }

    if(attributes.count("domain_id")){
        domain_id_ = attributes["domain_id"]->get_Integer();
    }

    std::cout << "ospl::InEventPort" << std::endl;
    std::cout << "**domain_id_: "<< domain_id_ << std::endl;
    std::cout << "**subscriber_name: "<< subscriber_name_ << std::endl;
    std::cout << "**topic_name_: "<< topic_name_ << std::endl << std::endl;


    if(topic_name_.length() > 0 && subscriber_name_.length() > 0){
        configured_ = true;
    }else{
        std::cout << "ospl::InEventPort<T, S>::startup: No Valid Topic_name + subscriber_names" << std::endl;
    }
    
};

template <class T, class S>
bool ospl::InEventPort<T, S>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    passivate_ = false;
    if(configured_){
        rec_thread_ = new std::thread(&ospl::InEventPort<T, S>::receive_loop, this);
    }
    return ::InEventPort<T>::Activate();
};

template <class T, class S>
bool ospl::InEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    passivate_ = true;
    if(rec_thread_){
        //Unlock the rec thread
        notify();

        //Join our zmq_thread
        rec_thread_->join();
        delete rec_thread_;
        rec_thread_ = 0;
    }
    
    return ::InEventPort<T>::Passivate();
};


template <class T, class S>
bool ospl::InEventPort<T, S>::Teardown(){
    Passivate();

    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
    return ::InEventPort<T>::Teardown();
};


template <class T, class S>
ospl::InEventPort<T, S>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function):
::InEventPort<T>(component, name, callback_function){
};

template <class T, class S>
void ospl::InEventPort<T, S>::receive_loop(){
    //Construct a DDS Participant, Subscriber, Topic and Reader
    auto helper = ospl::DdsHelper::get_dds_helper();    
    auto participant = helper->get_participant(domain_id_);
    auto subscriber = helper->get_subscriber(participant, subscriber_name_);
    auto topic = get_topic<S>(participant, topic_name_);
    auto reader_ = get_data_reader<S>(subscriber, topic);

    //Construct a DDS Listener, designed to call back into the receive thread
    auto listener_ = new ospl::DataReaderListener<T, S>(this);

    //Attach listener to only respond to data_available()
    reader_.listener(listener_, dds::core::status::StatusMask::data_available());

    while(true){
        {
            //Wait for next message
            std::unique_lock<std::mutex> lock(notify_mutex_);
            notify_lock_condition_.wait(lock);

            if(passivate_){
                break;
            }
        }

        ///Read all our samples
        auto samples = reader_.take();
        for(auto sample : samples){
            //Translate and callback into the component for each valid message we receive
            if(sample->info().valid()){
                auto m = translate(&sample->data());
                this->EnqueueMessage(m);
            }
        }
    }
};

#endif //OSPL_INEVENTPORT_H
