#ifndef QPID_INEVENTPORT_H
#define QPID_INEVENTPORT_H


#include "../globalinterfaces.h"
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Session.h>


namespace qpid{
    template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(::InEventPort<T>* port, std::string broker, std::string topic);
            void notify();
            void rx_(T* message);

        private:
            void receive_loop();
            void qpid_loop();

            qpid::messaging::Connection connection_;
            qpid::messaging::Session session_;
            qpid::messaging::Receiver receiver_;

            std::thread* receive_thread_;
            std::thread* qpid_thread_;
            std::mutex notify_mutex_;
            std::condition_variable notify_lock_condition_;

            std::queue<std::string> message_queue_;

            ::InEventPort<T>* port_;
    };
};


template <class T, class S>
void qpid::InEventPort<T, S>::rx_(T* message){
    if(port_){
        port_->rx_(message);
    }
};

template <class T, class S>
qpid::InEventPort<T, S>::InEventPort(::InEventPort<T>* port, std::string broker, std::string topic){
    this->port_ = port;

    connection_ = qpid::messaging::Connection(broker);
    connection_.open();
    session_ = connection_.createSession();
    //TODO: fix this to use actual topic name
    std::string tn = "amq.topic/" + topic;
    receiver_ = session_.createReceiver(tn);

    qpid_thread_ = new std::thread(&qpid::InEventPort<T,S>::qpid_loop, this);
    receive_thread_ = new std::thread(&qpid::InEventPort<T,S>::receive_loop, this);
};

template <class T, class S>
void qpid::InEventPort<T, S>::qpid_loop(){
    while(true){
        auto sample = receiver_.fetch();
        std::string str = sample.getContent();
        {
            //Gain mutex lock and append message
            std::unique_lock<std::mutex> lock(notify_mutex_);
            message_queue_.push(str);
            notify_lock_condition_.notify_all();
        }
    }
};

template <class T, class S>
void qpid::InEventPort<T, S>::receive_loop(){
    std::queue<std::string> queue_;
    while(true){
        {
            //Wait for next message
            std::unique_lock<std::mutex> lock(notify_mutex_);
            notify_lock_condition_.wait(lock);
            //Swap out the queue's and release the mutex
            message_queue_.swap(queue_);
        }
        while(!queue_.empty()){
            auto message = proto::decode(queue_.front());
            rx_(message);
            queue_.pop();
        }
    }
};

#endif