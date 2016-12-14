#ifndef ZMQ_INEVENTPORT_H
#define ZMQ_INEVENTPORT_H

#include "../globalinterfaces.h"
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include "zmqhelper.h"

namespace zmq{
     template <class T, class S> class Zmq_InEventPort: public ::InEventPort<T>{
        public:
            Zmq_InEventPort(::InEventPort<T>* port, std::vector<std::string> end_points);
            void rx_(T* message);
        private:
            void receive_loop();
            void zmq_loop();
            

            std::thread* zmq_thread_;
            std::thread* rec_thread_;

            ::InEventPort<T>* port_;

            std::vector<std::string> end_points_;

            std::mutex notify_mutex_;
            std::condition_variable notify_lock_condition_;
            std::queue<std::string> message_queue_;
    }; 
};

template <class T, class S>
void zmq::Zmq_InEventPort<T, S>::rx_(T* message){
    if(port_){
        port_->rx_(message);
    }
};

template <class T, class S>
void zmq::Zmq_InEventPort<T, S>::receive_loop(){
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
            auto m = proto::decode(queue_.front());
            rx_(m);
            queue_.pop();
        }
    }
};

template <class T, class S>
void zmq::Zmq_InEventPort<T, S>::zmq_loop(){
    auto helper = ZmqHelper::get_zmq_helper();
    auto socket = helper->get_subscriber_socket();

    for(auto end_point: end_points_){
        //std::cout << "Binding To: " << end_point << std::endl;
        //Connect to the publisher
        socket->connect(end_point.c_str());   
    }

    //Construct a new ZMQ Message to store the resulting message in.
    zmq::message_t *data = new zmq::message_t();

    while(true){
		try{
            //Wait for next message
            socket->recv(data);
            std::cout << "Got Message:" << std::endl;
            
            //If we have a valid message
            if(data->size() > 0){
                //Construct a string out of the zmq data
                std::string msg_str(static_cast<char *>(data->data()), data->size());
                {
                    //Gain mutex lock and append message
                    std::unique_lock<std::mutex> lock(notify_mutex_);
                    message_queue_.push(msg_str);
                    notify_lock_condition_.notify_all();
                }
            }
        }catch(zmq::error_t ex){
            //Do nothing with an error.
			continue;
        }
    }
};

template <class T, class S>
zmq::Zmq_InEventPort<T, S>::Zmq_InEventPort(::InEventPort<T>* port, std::vector<std::string> end_points){
    this->port_ = port;
    this->end_points_ = end_points;

    zmq_thread_ = new std::thread(&zmq::Zmq_InEventPort<T, S>::zmq_loop, this);
    rec_thread_ = new std::thread(&zmq::Zmq_InEventPort<T, S>::receive_loop, this);
};

#endif //ZMQ_INEVENTPORT_H
