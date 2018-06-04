#ifndef ZMQ_PORT_REPLIER_HPP
#define ZMQ_PORT_REPLIER_HPP

#include <core/ports/requestreply/replierport.hpp>
#include <middleware/proto/prototranslator.h>
#include <middleware/zmq/zmqhelper.h>
#include <re_common/zmq/zmqutils.hpp>

#include <thread>
#include <mutex>

namespace zmq{
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    class ReplierPort : public ::ReplierPort<BaseReplyType, BaseRequestType>{
        public:
            ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > server_function);
            ~ReplierPort(){
                Activatable::Terminate();
            };
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            bool TerminateThread();

            void recv_loop();
            
            std::thread* recv_thread_ = 0;
            std::string terminate_endpoint_;
            std::shared_ptr<Attribute> end_point_;

            //Translators
            ::Proto::Translator<BaseReplyType, ProtoReplyType> reply_translator;
            ::Proto::Translator<BaseRequestType, ProtoRequestType> request_translator;

            std::mutex thread_state_mutex_;
            ThreadState thread_state_ = ThreadState::TERMINATED;
            std::condition_variable thread_state_condition_;
            
            std::mutex control_mutex_;
            
            const std::string terminate_str = "TERMINATE";
            bool passivated_ = false;
    }; 
};


template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name,  std::function<BaseReplyType (BaseRequestType&) > server_function):
::ReplierPort<BaseReplyType, BaseRequestType>(component, port_name, server_function, "zmq"){
    auto component_ = component.lock();
    auto component_name = component_ ? component_->get_name() : "??";
    auto component_id = component_ ? component_->get_id() : "??";
    terminate_endpoint_ = "inproc://term*" + component_name + "*" + port_name + "*" + component_id + "*";

    end_point_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = end_point_->String().size() >= 0;
    if(valid && ::ReplierPort<BaseReplyType, BaseRequestType>::HandleConfigure()){
        if(!recv_thread_){
            //Construct the thread, and wait for the thread state to be changed
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            recv_thread_ = new std::thread(&zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::recv_loop, this);
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
}


template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Call into the base class
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandleTerminate()){
        //Terminate the thread
        TerminateThread();

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

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Call into the base Handle Passivate
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate()){
        return TerminateThread();
    }
    return false;
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::recv_loop(){
    auto socket = ZmqHelper::get_zmq_helper()->get_reply_socket();
    auto state = ThreadState::STARTED;

    
    try{
        socket.bind(terminate_endpoint_.c_str());
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to terminate endpoint: '" + terminate_endpoint_ + "' " + ex.what());
        state = ThreadState::TERMINATED;
    }

    if(state != ThreadState::TERMINATED){
        const auto address = end_point_->String();
        try{
            socket.bind(address.c_str());
        }catch(const zmq::error_t& ex){
            Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to endpoint: '" + address + "' " + ex.what());
            state = ThreadState::TERMINATED;
        }
    }

    if(state != ThreadState::TERMINATED){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    {
        //Update the State Variable
        std::lock_guard<std::mutex> lock(thread_state_mutex_);
        thread_state_ = state;
        thread_state_condition_.notify_all();
    }

    if(state == ThreadState::STARTED && Activatable::BlockUntilStateChanged(Activatable::State::RUNNING)){
        //Log the port becoming online
        ::Port::LogActivation();

        while(true){
            try{
                //Wait for next message
                zmq::message_t zmq_request;
                socket.recv(&zmq_request);
                const auto& request_str = Zmq2String(zmq_request);
                
                if(request_str == terminate_str){
                    std::cerr << "GOT TERMINATE" << std::endl;
                    //Send back a Terminate
                    socket.send(zmq_request);
                    break;
                }
                //Translate the request string into a Base Message
                auto request_message = request_translator.StringToBase(request_str);

                //Run through the callback
                auto reply_message = ::ReplierPort<BaseReplyType, BaseRequestType>::ProcessRequest(*request_message);

                //Convert back to String
                auto reply_str = reply_translator.BaseToString(reply_message);
                //Convert back to ZMQ
                zmq::message_t zmq_reply(reply_str.c_str(), reply_str.size());
                socket.send(zmq_reply);
            }catch(zmq::error_t ex){
                std::cerr << "ERROR FAM" << std::endl;
                Log(Severity::ERROR_).Context(this).Func(__func__).Msg(ex.what());
                break;
            }
        }
        ::Port::LogPassivation();
    }

    //Change the state to be Terminated
    std::lock_guard<std::mutex> lock(thread_state_mutex_);
    thread_state_ = ThreadState::TERMINATED;
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::TerminateThread(){
    std::lock_guard<std::mutex> lock(thread_state_mutex_);

    switch(thread_state_){
        case ThreadState::STARTED:{
            try{
                auto socket = ZmqHelper::get_zmq_helper()->get_request_socket();
            
                //Connect to the terminate address
                socket.connect(terminate_endpoint_.c_str());
                
                //Send the special terminate message
                zmq::message_t term_msg(terminate_str.c_str(), terminate_str.size());
                socket.send(term_msg);
                //Wait for the response
                socket.recv(&term_msg);
                return true;
            }catch(const zmq::error_t& ex){
                Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to Terminate ZMQ Server Port") + ex.what());
            }
            break;
        }
        case ThreadState::TERMINATED:{
            return true;
        }
        default:
            break;
    }
    return false;
}

#endif //ZMQ_PORT_REPLIER_HPP
