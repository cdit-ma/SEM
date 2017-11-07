#ifndef ZMQ_INEVENTPORT_H
#define ZMQ_INEVENTPORT_H

#include <core/eventports/ineventport.hpp>
#include "zmqhelper.h"

#include <thread>
#include <mutex>

namespace zmq{
     template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function);
            ~InEventPort(){
                Passivate();
                Teardown();
            }
            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();
            bool Passivate();

        private:
            void zmq_loop();

            std::thread* zmq_thread_ = 0;
            std::string terminate_endpoint_;
            std::shared_ptr<Attribute> end_points_;

            std::mutex control_mutex_;
            const std::string terminate_str = "TERMINATE";
    }; 
};

template <class T, class S>
void zmq::InEventPort<T, S>::zmq_loop(){
    auto helper = ZmqHelper::get_zmq_helper();
    auto socket = helper->get_subscriber_socket();

    try{
        socket->connect(terminate_endpoint_.c_str());    
        for(auto e: end_points_->StringList()){
            //std::cout << "zmq::InEventPort<T, S>::zmq_loop(): " << this->get_name() << " Connecting To: " << end_point << std::endl;
            //Connect to the publisher
            socket->connect(e.c_str());   
        }
    }catch(zmq::error_t ex){
        std::cerr << "zmq::InEventPort<T, S>::zmq_loop(): Couldn't connect to endpoints!" << std::endl;
    }
    //Construct a new ZMQ Message to store the resulting message in.
    zmq::message_t data;

    //Sleep for 250 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    
    //Notify Startup our thread is good to go
    Activatable::StartupFinished();
    //Wait for the port to be activated before starting!
    Activatable::WaitForActivate();
    //Log the port becoming online
    EventPort::LogActivation();


    while(true){
		try{
            //Wait for next message
            socket->recv(&data);

            std::string msg_str(static_cast<char *>(data.data()), data.size());
            
            
            if(msg_str.size() == terminate_str.size()){
                if(msg_str == terminate_str){
                    break;
                }
            }
            
            auto m = proto::decode<S>(msg_str);
            this->EnqueueMessage(m);

        }catch(zmq::error_t ex){
            //Do nothing with an error.
            std::cerr << "zmq::InEventPort<T, S>::zmq_loop(): ZMQ_Error: " << ex.num() << std::endl;
			break;
        }
    }
    EventPort::LogPassivation();
    delete socket;
};

template <class T, class S>
zmq::InEventPort<T, S>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function):
::InEventPort<T>(component, name, callback_function, "zmq"){
    terminate_endpoint_ = "inproc://term*" + component->get_name() + "*" + name + "*";
    end_points_ = ::InEventPort<T>::AddAttribute(std::make_shared<Attribute>(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address"));
};


template <class T, class S>
void zmq::InEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = end_points_->StringList().size() > 0;
    if(valid){
        if(!zmq_thread_){
            zmq_thread_ = new std::thread(&zmq::InEventPort<T, S>::zmq_loop, this);
            Activatable::WaitForStartup();
        }
    }else{
        std::cerr << "zmq::InEventPort<T, S>::startup: No Valid Endpoints" << std::endl;
    }
};

template <class T, class S>
bool zmq::InEventPort<T, S>::Teardown(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::Teardown()){
        if(zmq_thread_){
            //Join our zmq_thread
            zmq_thread_->join();
            delete zmq_thread_;
            zmq_thread_ = 0;
        }
        return true;
    }
    return false;
};


template <class T, class S>
bool zmq::InEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::Passivate()){
        if(zmq_thread_){
            //Construct our terminate socket
            auto helper = ZmqHelper::get_zmq_helper();
            auto term_socket = helper->get_publisher_socket();
            term_socket->bind(terminate_endpoint_.c_str());

            zmq::message_t term_msg(terminate_str.c_str(), terminate_str.size());
            //Send a blank message to interupt the recv loop
            term_socket->send(term_msg);
            delete term_socket;
        }
        return true;
    }
    return false;
};





#endif //ZMQ_INEVENTPORT_H
