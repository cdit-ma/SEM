#ifndef ZMQ_OUTEVENTPORT_H
#define ZMQ_OUTEVENTPORT_H

#include <core/eventports/prototranslator.h>
#include <core/eventports/outeventport.hpp>
#include <re_common/zmq/zmqutils.hpp>
#include "zmqhelper.h"

namespace zmq{
     template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(std::weak_ptr<Component> component, std::string name);
            ~OutEventPort(){
                Activatable::Terminate();
            }
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        public:
            bool tx(const T& message);
        private:
            bool setup_tx();
            std::mutex control_mutex_;

            ::Proto::Translator<T, S> translater;
            
            zmq::socket_t* socket_ = 0;
            std::shared_ptr<Attribute> end_points_;
    }; 
};

template <class T, class S>
zmq::OutEventPort<T, S>::OutEventPort(std::weak_ptr<Component> component, std::string name): ::OutEventPort<T>(component, name, "zmq"){
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
};



template <class T, class S>
bool zmq::OutEventPort<T, S>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool valid = end_points_->StringList().size() > 0;

    if(valid && ::OutEventPort<T>::HandleConfigure()){
        return setup_tx();
    }
    return false;
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::HandlePassivate()){
        if(socket_){
            delete socket_;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            socket_ = 0;

        }
        return true;
    }
    return false;
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    return ::OutEventPort<T>::HandleTerminate();
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::setup_tx(){
    auto helper = ZmqHelper::get_zmq_helper();
    this->socket_ = helper->get_publisher_socket();
    for(auto e: end_points_->StringList()){
        try{
            //Bind the addresses provided
            this->socket_->bind(e.c_str());
        }catch(zmq::error_t ex){
            Log(Severity::ERROR_).Context(this).Func(GET_FUNC).Msg("Cannot bind endpoint: '" + e + "' " + ex.what());
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::tx(const T& message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool should_send = ::OutEventPort<T>::tx(message);

    if(should_send){
        if(socket_){
            auto str = translater.BaseToString(message);
            zmq::message_t data(str.c_str(), str.size());
            socket_->send(data);
            return true;
        }else{
            Log(Severity::DEBUG).Context(this).Func(GET_FUNC).Msg("Socket unexpectedly null");
        }
    }
    return false;
};

#endif //ZMQ_INEVENTPORT_H

