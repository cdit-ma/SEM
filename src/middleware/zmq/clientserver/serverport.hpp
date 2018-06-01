#ifndef ZMQ_SERVER_EVENTPORT_H
#define ZMQ_SERVER_EVENTPORT_H

#include <core/eventports/prototranslator.h>
#include <core/eventports/clientserver/serverport.hpp>
#include <re_common/zmq/zmqutils.hpp>
#include "../zmqhelper.h"

#include <thread>
#include <mutex>

namespace zmq{
    template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
    class ServerEventPort : public ::ClientEventPort<BaseReplyType, BaseRequestType>{
        public:
            ServerEventPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<ReturnType (RequestType&) > server_function);
            ~ServerEventPort(){
                Activatable::Terminate();
            }
            BaseReplyType tx(const BaseRequestType& type);
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            void recv_loop();
            
            std::thread* recv_thread_ = 0;
            std::string terminate_endpoint_;
            std::shared_ptr<Attribute> end_point_;

            //Translators
            ::Proto::Translator<BaseReplyType, ReplyType> reply_translater;
            ::Proto::Translator<BaseRequestType, RequestType> request_translater;

            std::mutex thread_state_mutex_;
            ThreadState thread_state_;
            std::condition_variable thread_state_condition_;
            
            std::mutex control_mutex_;
            
            std::mutex terminate_mutex_;
            zmq::socket_t* term_socket_ = 0;
            const std::string terminate_str = "TERMINATE";
    }; 
};


template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
zmq::ServerEventPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::ServerEventPort(std::weak_ptr<Component> component, const std::string& port_name,  std::function<ReturnType (RequestType&) > server_function):
::ServerEventPort<BaseReplyType, BaseRequestType>(component, name, server_function "zmq"){
    auto component_ = component.lock();
    auto component_name = component_ ? component_->get_name() : "??";
    auto component_id = component_ ? component_->get_id() : "??";
    terminate_endpoint_ = "inproc://term*" + component_name + "*" + name + "*" + component_id + "*";

    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
};

template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
zmq::ServerEventPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = end_points_->StringList().size() >= 0;
    if(valid && ::ServerEventPort<BaseReplyType, BaseRequestType>::HandleConfigure()){
        if(!recv_thread_){
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            recv_thread_ = new std::thread(&zmq::ServerEventPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::recv_loop, this);
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
}


template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
zmq::ServerEventPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::ServerEventPort<BaseReplyType, BaseRequestType>::HandleTerminate()){
        if(recv_thread_){
            //Join our zmq_thread
            recv_thread_->join();
            delete recv_thread_;
            recv_thread_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
zmq::ServerEventPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::ServerEventPort<BaseReplyType, BaseRequestType>::HandlePassivate()){
        std::lock_guard<std::mutex> lock(terminate_mutex_);
        if(term_socket_){
            zmq::message_t term_msg(terminate_str.c_str(), terminate_str.size());
            //Send a blank message to interupt the recv loop
            term_socket_->send(term_msg);
        }
        return true;
    }
    return false;
};

template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
zmq::ServerEventPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::recv_loop(){
    terminate_mutex_.lock();
    auto helper = ZmqHelper::get_zmq_helper();
    auto socket = helper->get_reply_socket();
    term_socket_ = helper->get_request_socket();

    auto state = ThreadState::STARTED;

    //Bind to the Socket
    for(auto e: end_points_->StringList()){
        try{
            //connect the addresses provided
            socket->bind(e.c_str());
        }catch(zmq::error_t ex){
            Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot bind to endpoint: '" + e + "' " + ex.what());
            state = ThreadState::ERROR_;
        }
    }

    try{
        socket->bind(terminate_endpoint_.c_str());
        term_socket_->connect(terminate_endpoint_.c_str());
    }catch(zmq::error_t ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to terminate endpoint: '" + terminate_endpoint_ + "' " + ex.what());
        state = ThreadState::ERROR_;
    }

    

    //Sleep for 250 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //Release the temrinate mutex
    terminate_mutex_.unlock();

    //Change the state to be Configured
    {
        std::lock_guard<std::mutex> lock(thread_state_mutex_);
        thread_state_ = state;
        thread_state_condition_.notify_all();
    }

    if(state == ThreadState::STARTED && Activatable::BlockUntilStateChanged(Activatable::State::RUNNING)){
        //Log the port becoming online
        EventPort::LogActivation();

        
    
        while(true){
            try{
                //Wait for next message
                zmq::message_t request_data;
                socket->recv(&request_data);
                
                auto request_string = Zmq2String(request_data);
                
                if(request_string == terminate_str){
                    break;
                }

                auto request_message = request_translator.StringToBase(request_string);
                auto reply_message = rx(*request_message);

                auto reply_string = reply_translator.BaseToString(reply_message);

                zmq::message_t reply_data(reply_string.c_str(), reply_string.size());
                socket_->send(reply_data);
            }catch(zmq::error_t ex){
                Log(Severity::ERROR_).Context(this).Func(__func__).Msg(ex.what());
                break;
            }
        }
        EventPort::LogPassivation();
    }

    delete socket;


    //Gain the terminate_socket to kill
    std::lock_guard<std::mutex> lock(terminate_mutex_);
    delete term_socket_;
    term_socket_ = 0;
};

#endif //ZMQ_SERVER_EVENTPORT_H
