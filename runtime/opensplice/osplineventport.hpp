#ifndef OSPL_INEVENTPORT_H
#define OSPL_INEVENTPORT_H

#include "../globalinterfaces.h"
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>

#include "osplhelper.h"
#include "datareaderlistener.hpp"

namespace ospl{
     template <class T, class S> class Ospl_InEventPort: public ::OutEventPort<T>{
        public:
            Ospl_InEventPort(::InEventPort<T>* port, int domain_id, std::string subscriber_name, std::string reader_name, std::string topic_name);
            void rx_(T* message);
        private:
            void reciever_loop();
            
            std::thread* rec_thread_;
            std::mutex notify_mutex_;
            std::condition_variable notify_lock_condition_;
            

            DataReaderListener<T, S> listener_;
            dds::sub::DataReader<S> reader_;
            ::OutEventPort<T>* port_;
    }; 
};

template <class T, class S>
void ospl::Ospl_InEventPort<T, S>::tx_(T* message){
    if(port_){
        port_->rx_(message);
    }
};

template <class T, class S>
void ospl::Ospl_InEventPort<T, S>::notify(){
    //Notify thread
    std::unique_lock<std::mutex> lock(notify_mutex_);
    notify_lock_condition_.notify_all();
};


template <class T, class S>
ospl::OutEventPort<T, S>::Ospl_InEventPort(::OutEventPort<T>* port, int domain_id, std::string subscriber_name, std::string reader_name, std::string topic_name){
    this->port_ = port;
    
    auto helper = OsplHelper::get_ospl_helper();   
    auto participant = helper->get_participant(domain_id);
    auto subscriber = helper->get_subscriber(participant, subscriber_name);
    auto topic = helper->get_topic<S>(participant, topic_name);
    reader_ = helper->get_data_reader<S>(subscriber, topic, reader_name);

    listener_ = new DataReaderListener<T, S>(this);

    //Only listen to data-available
    reader_.listener(listener_, dds::core::status::StatusMask::data_available());

    //Setup Thread
    rec_thread_ = new std::thread(&RxMessage::recieve_loop, this);
};

template <class T, class S>
ospl::OutEventPort<T, S>::recieve_loop(){  
    while(true){
        {
            //Wait for next message
            std::unique_lock<std::mutex> lock(notify_mutex_);
            notify_lock_condition_.wait(lock);
        }

        auto samples = reader.take();
        for(auto sample_it = samples.begin(); sample_it != samples.end(); ++sample_it){
            //Recieve our valid samples
            if(sample_it->info().valid()){
                auto m = translate(&sample_it->data());
                rx_(m);
            }
        }
    }
}

#endif //OSPL_INEVENTPORT_H
