#ifndef OSPL_INEVENTPORT_H
#define OSPL_INEVENTPORT_H

#include "../core/eventports/ineventport.hpp"

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "helper.hpp"
#include "datareaderlistener.hpp"

namespace ospl{
     template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(Component* component, std::function<void (T*) > callback_function, int domain_id, std::string subscriber_name, std::string topic_name);
            void notify();
        private:
            void receive_loop();
            
            std::thread* rec_thread_;
            std::mutex notify_mutex_;
            std::condition_variable notify_lock_condition_;
            
            ospl::DataReaderListener<T,S>* listener_;
            dds::sub::DataReader<S> reader_ = dds::sub::DataReader<S>(dds::core::null);
    }; 
};


template <class T, class S>
void ospl::InEventPort<T, S>::notify(){
    //Called by the DataReaderListener to notify our InEventPort thread to get new data
    std::unique_lock<std::mutex> lock(notify_mutex_);
    notify_lock_condition_.notify_all();
};


template <class T, class S>
ospl::InEventPort<T, S>::InEventPort(Component* component, std::function<void (T*) > callback_function, int domain_id, std::string subscriber_name, std::string topic_name):
::InEventPort<T>(component, callback_function){
    //Construct a DDS Participant, Subscriber, Topic and Reader
    auto helper = DdsHelper::get_dds_helper();    
    auto participant = helper->get_participant(domain_id);
    auto subscriber = helper->get_subscriber(participant, subscriber_name);
    auto topic = helper->get_topic<S>(participant, topic_name);
    reader_ = helper->get_data_reader<S>(subscriber, topic);

    //Construct a DDS Listener, designed to call back into the receive thread
    listener_ = new ospl::DataReaderListener<T, S>(this);

    //Attach listener to only respond to data_available()
    reader_.listener(listener_, dds::core::status::StatusMask::data_available());

    //Setup a receiver thread, so that we don't tie up the middlewares callback thread
    rec_thread_ = new std::thread(&ospl::InEventPort<T, S>::receive_loop, this);
};

template <class T, class S>
void ospl::InEventPort<T, S>::receive_loop(){  
    while(true){
        {
            //Wait for next message
            std::unique_lock<std::mutex> lock(notify_mutex_);
            notify_lock_condition_.wait(lock);
        }

        ///Read all our samples
        auto samples = reader_.take();
        for(auto sample : samples){
            //Translate and callback into the component for each valid message we receive
            if(sample->info().valid()){
                auto m = translate(&sample->data());
                this->rx(m);
            }
        }
    }
};

#endif //OSPL_INEVENTPORT_H
