#ifndef RTI_INEVENTPORT_H
#define RTI_INEVENTPORT_H

#include "../globalinterfaces.h"
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>


#include "helper.hpp"
#include "datareaderlistener.hpp"

namespace rti{
     template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(::InEventPort<T>* port, int domain_id, std::string subscriber_name, std::string topic_name);
            void notify();
            void rx_(T* message);
        private:
            void recieve_loop();
            
            std::thread* rec_thread_;
            std::mutex notify_mutex_;
            std::condition_variable notify_lock_condition_;
            

            rti::DataReaderListener<T,S>* listener_;
            dds::sub::DataReader<S> reader_ = dds::sub::DataReader<S>(dds::core::null);
            ::InEventPort<T>* port_;
    }; 
};

template <class T, class S>
void rti::InEventPort<T, S>::rx_(T* message){
    if(port_){
        port_->rx_(message);
    }
};

template <class T, class S>
void rti::InEventPort<T, S>::notify(){
    //Notify thread
    std::unique_lock<std::mutex> lock(notify_mutex_);
    notify_lock_condition_.notify_all();
};


template <class T, class S>
rti::InEventPort<T, S>::InEventPort(::InEventPort<T>* port, int domain_id, std::string subscriber_name, std::string topic_name){
    this->port_ = port;
    
    auto helper = DdsHelper::get_dds_helper();    
    auto participant = helper->get_participant(domain_id);
    auto subscriber = helper->get_subscriber(participant, subscriber_name);
    auto topic = helper->get_topic<S>(participant, topic_name);
    reader_ = helper->get_data_reader<S>(subscriber, topic);

    listener_ = new rti::DataReaderListener<T, S>(this);

    //Only listen to data-available
    reader_.listener(listener_, dds::core::status::StatusMask::data_available());

    //Setup Thread
    rec_thread_ = new std::thread(&rti::InEventPort<T,S>::recieve_loop, this);
};

template <class T, class S>
void rti::InEventPort<T, S>::recieve_loop(){  
    while(true){
        {
            //Wait for next message
            std::unique_lock<std::mutex> lock(notify_mutex_);
            notify_lock_condition_.wait(lock);
        }

        auto samples = reader_.take();
        for(auto sample_it = samples.begin(); sample_it != samples.end(); ++sample_it){
            //Recieve our valid samples
            if(sample_it->info().valid()){
                auto m = translate(&sample_it->data());
                rx_(m);
            }
        }
    }
};

#endif //RTI_INEVENTPORT_H
