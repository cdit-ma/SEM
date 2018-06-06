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

            //Translators
            ::Proto::Translator<BaseReplyType, ProtoReplyType> reply_translator;
            ::Proto::Translator<BaseRequestType, ProtoRequestType> request_translator;


            std::mutex connection_mutex_;
            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Receiver receiver_ = 0;

            std::mutex thread_state_mutex_;
            ThreadState thread_state_ = ThreadState::TERMINATED;
            std::condition_variable thread_state_condition_;
            
            std::mutex control_mutex_;
            
            const std::string terminate_str = "TERMINATE";
            bool passivated_ = false;

            std::shared_ptr<Attribute> topic_name_;
            std::shared_ptr<Attribute> broker_;
    }; 
};


template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name,  std::function<BaseReplyType (BaseRequestType&) > server_function):
::ReplierPort<BaseReplyType, BaseRequestType>(component, port_name, server_function, "zmq"){
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
            recv_thread_ = new std::thread(&qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::recv_loop, this);
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
void qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::recv_loop(){
    auto state = ThreadState::STARTED;

    qpid::messaging::Session session;

    try{
        std::lock_guard<std::mutex> lock(connection_mutex_);
        //Construct a Qpid Connection
        connection_ = qpid::messaging::Connection(broker_->String());
        //Open the connection
        connection_.open();
        session = connection_.createSession();
        receiver_ = session.createReceiver("amq.topic/reqrep/"  + topic_name_->String());
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to startup QPID AMQP Reciever") + ex.what());
        state = ThreadState::TERMINATED;
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

        bool run = true;

        while(run){
            try{
                //Wait for next message

                auto request = receiver_.fetch();
                if(receiver_.isClosed()){
                    run = false;
                }
                const auto& address = request.getReplyTo();
                if(address){
                    const auto& request_string = request.getContent();
                    auto request_message = request_translator.StringToBase(request_string);
                    ::Port::EventRecieved(*request_message);
                    

                    auto reply_message = ::ReplierPort<BaseReplyType, BaseRequestType>::ProcessRequest(*request_message);
                    auto reply_string = reply_translator.BaseToString(reply_message);

                    auto reply_sender = session.createSender(address);

                    qpid::messaging::Message reply;
                    
                    reply.setContentObject(reply_string);
                    
                    reply_sender.send(reply);
                    
                    session.acknowledge();
                }
            }catch(const std::exception& ex){
                //std::cerr << "ERROR qpid::ReplierPort::recv_loop()" << std::endl;
                Log(Severity::ERROR_).Context(this).Func(__func__).Msg(ex.what());
                run = false;
            }
        }
        ::Port::LogPassivation();
    }

    //Change the state to be Terminated
    std::lock_guard<std::mutex> lock(thread_state_mutex_);
    thread_state_ = ThreadState::TERMINATED;
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::TerminateThread(){
    std::lock_guard<std::mutex> lock(thread_state_mutex_);

    switch(thread_state_){
        case ThreadState::STARTED:{
            try{
                if(connection_.isOpen()){
                    receiver_.close();
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
}

#endif //QPID_PORT_REPLIER_HPP