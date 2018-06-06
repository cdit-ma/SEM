#ifndef QPID_PORT_REPLIER_HPP
#define QPID_PORT_REPLIER_HPP

#include <core/ports/requestreply/replierport.hpp>
#include <middleware/proto/prototranslator.h>

#include <thread>
#include <mutex>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Duration.h>
#include <qpid/messaging/Sender.h>

namespace qpid{
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    struct RequestHandler;

    //Generic templated ReplierPort
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    class ReplierPort : public ::ReplierPort<BaseReplyType, BaseRequestType>{
        //The Request Handle needs to be able to modify and change state of the Port
        friend class RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>;
        public:
            ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > server_function);
            ~ReplierPort(){
                Activatable::Terminate();
            };
            using middleware_reply_type = ProtoReplyType;
            using middleware_request_type = ProtoRequestType;
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            bool TerminateThread();
            
            std::thread* recv_thread_ = 0;

            std::mutex connection_mutex_;
            qpid::messaging::Connection connection_ = 0;

            std::mutex thread_state_mutex_;
            ThreadState thread_state_ = ThreadState::TERMINATED;
            std::condition_variable thread_state_condition_;
            
            std::mutex control_mutex_;
            std::shared_ptr<Attribute> topic_name_;
            std::shared_ptr<Attribute> broker_;
    };

    //Generic templated RequesterHandler
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    struct RequestHandler{
        static void Loop(qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port);
    };
    
    //Specialised templated RequesterHandler for void returning
    template <class BaseRequestType, class ProtoRequestType>
    struct RequestHandler<void, void, BaseRequestType, ProtoRequestType>{
        static void Loop(qpid::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port);
    };
};

//Generic templated ReplierPort
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name,  std::function<BaseReplyType (BaseRequestType&) > server_function):
::ReplierPort<BaseReplyType, BaseRequestType>(component, port_name, server_function, "qpid"){
    auto component_ = component.lock();
    auto component_name = component_ ? component_->get_name() : "??";
    auto component_id = component_ ? component_->get_id() : "??";

    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandleConfigure()){
        if(!recv_thread_){
            //Construct the thread, and wait for the thread state to be changed
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            recv_thread_ = new std::thread(qpid::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop, std::ref(*this));
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
}


template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleTerminate(){
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
bool qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Call into the base Handle Passivate
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate()){
        return TerminateThread();
    }
    return false;
};


template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::TerminateThread(){
    std::lock_guard<std::mutex> lock(thread_state_mutex_);

    switch(thread_state_){
        case ThreadState::STARTED:{
            try{
                std::lock_guard<std::mutex> lock(connection_mutex_);
                if(connection_.isOpen()){
                    connection_.close();
                }
                return true;
            }catch(const std::exception& ex){
                Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to close qpid receiver.") + ex.what());
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
};

//Generic templated RequestHandler
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void qpid::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop(qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port){
    auto state = ThreadState::STARTED;

    qpid::messaging::Session session;
    qpid::messaging::Receiver receiver;

    const auto& broker_address = port.broker_->String();
    const auto& topic_name = port.topic_name_->String();

    try{
        std::lock_guard<std::mutex> lock(port.connection_mutex_);
        //Construct a Qpid Connection
        port.connection_ = qpid::messaging::Connection(broker_address);
        //Open the connection
        port.connection_.open();
        session = port.connection_.createSession();
        receiver = session.createReceiver("amq.topic/reqrep/"  + topic_name);
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(std::string("Unable to startup QPID AMQP Reciever") + ex.what());
        state = ThreadState::TERMINATED;
    }
    
    {
        //Update the Port's state variable 
        std::lock_guard<std::mutex> lock(port.thread_state_mutex_);
        port.thread_state_ = state;
        port.thread_state_condition_.notify_all();
    }

    if(state == ThreadState::STARTED && port.BlockUntilStateChanged(Activatable::State::RUNNING)){
        //Log the port becoming online
        port.LogActivation();

        while(true){
            try{
                //Wait for next message
                auto request = receiver.fetch();
                
                const auto& address = request.getReplyTo();
                if(address){
                    const auto& request_string = request.getContent();
                    auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(request_str);
                    //Call through the base ProcessRequest function, which calls any attached callback
                    auto base_reply = port.ProcessRequest(*base_request_ptr);
                    auto reply_str = ::Proto::Translator<BaseReplyType, ProtoReplyType>::BaseToString(base_reply);

                    //Create a Sender to send back
                    auto reply_sender = session.createSender(address);
                    reply_sender.send(qpid::messaging::Message(reply_str));
                    session.acknowledge();
                }
            }catch(const std::exception& ex){
                Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                break;
            }
        }
        //Log that the port has been passivated
        port.LogPassivation();
    }

    //Change the state to be Terminated
    std::lock_guard<std::mutex> lock(port.thread_state_mutex_);
    port.thread_state_ = ThreadState::TERMINATED;
};


//Specialised templated RequestHandler for void returning
template <class BaseRequestType, class ProtoRequestType>
void qpid::RequestHandler<void, void, BaseRequestType, ProtoRequestType>::Loop(qpid::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port){
    auto state = ThreadState::STARTED;

    qpid::messaging::Session session;
    qpid::messaging::Receiver receiver;

    const auto& broker_address = port.broker_->String();
    const auto& topic_name = port.topic_name_->String();

    try{
        std::lock_guard<std::mutex> lock(port.connection_mutex_);
        //Construct a Qpid Connection
        port.connection_ = qpid::messaging::Connection(broker_address);
        //Open the connection
        port.connection_.open();
        session = port.connection_.createSession();
        receiver = session.createReceiver("amq.topic/reqrep/"  + topic_name);
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(std::string("Unable to startup QPID AMQP Reciever") + ex.what());
        state = ThreadState::TERMINATED;
    }
    
    {
        //Update the Port's state variable 
        std::lock_guard<std::mutex> lock(port.thread_state_mutex_);
        port.thread_state_ = state;
        port.thread_state_condition_.notify_all();
    }

    if(state == ThreadState::STARTED && port.BlockUntilStateChanged(Activatable::State::RUNNING)){
        //Log the port becoming online
        port.LogActivation();

        while(true){
            try{
                //Wait for next message
                auto request = receiver.fetch();
                
                const auto& address = request.getReplyTo();
                if(address){
                    const auto& request_string = request.getContent();
                    auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(request_str);
                    //Call through the base ProcessRequest function, which calls any attached callback
                    port.ProcessRequest(*base_request_ptr);
                    session.acknowledge();
                }
            }catch(const std::exception& ex){
                Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                break;
            }
        }
        //Log that the port has been passivated
        port.LogPassivation();
    }

    //Change the state to be Terminated
    std::lock_guard<std::mutex> lock(port.thread_state_mutex_);
    port.thread_state_ = ThreadState::TERMINATED;
};

#endif //QPID_PORT_REPLIER_HPP