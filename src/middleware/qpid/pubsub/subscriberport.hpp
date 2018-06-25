#ifndef QPID_PORT_SUBSCRIBER_HPP
#define QPID_PORT_SUBSCRIBER_HPP

#include <core/threadmanager.h>
#include <middleware/proto/prototranslator.h>
#include <core/ports/pubsub/subscriberport.hpp>

#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <functional>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Duration.h>


namespace qpid{
    template <class BaseType, class ProtoType>
    struct SubscriberHandler;

    template <class BaseType, class ProtoType> class SubscriberPort: public ::SubscriberPort<BaseType>{
        friend class SubscriberHandler<BaseType, ProtoType>;
        public:
            SubscriberPort(std::weak_ptr<Component> component, std::string name, const CallbackWrapper<void, BaseType> callback_wrapper);
            ~SubscriberPort(){
                Activatable::Terminate();
            }

            protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
            bool HandleActivate();
        private:
            bool TerminateThread();

            ::Proto::Translator<BaseType, ProtoType> translator;

            std::mutex connection_mutex_;
            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Receiver receiver_ = 0;

            ThreadManager* thread_manager_ = 0;

            std::mutex control_mutex_;
            std::thread* recv_thread_ = 0;

            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;
    };

    template <class BaseType, class ProtoType>
    struct SubscriberHandler{
        static void Loop(ThreadManager& thread_manager, qpid::SubscriberPort<BaseType, ProtoType>& port);
    };
};

template <class BaseType, class ProtoType>
qpid::SubscriberPort<BaseType, ProtoType>::SubscriberPort(std::weak_ptr<Component> component, std::string name, const CallbackWrapper<void, BaseType> callback_wrapper):
::SubscriberPort<BaseType>(component, name, callback_wrapper, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseType, class ProtoType>
bool qpid::SubscriberPort<BaseType, ProtoType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = topic_name_->String().length() >= 0 && broker_->String().length() >= 0;
    if(valid && ::SubscriberPort<BaseType>::HandleConfigure()){
        if(!thread_manager_){
            thread_manager_ = new ThreadManager();
            auto thread = std::unique_ptr<std::thread>(new std::thread(qpid::SubscriberHandler<BaseType, ProtoType>::Loop, std::ref(*thread_manager_), std::ref(*this)));
            thread_manager_->SetThread(std::move(thread));
            return thread_manager_->Configure();
        }
        else{
            std::cerr << "Have extra thread manager" << std::endl;
        }
    }
    return false;
};

template <class BaseType, class ProtoType>
bool qpid::SubscriberPort<BaseType, ProtoType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    if(::SubscriberPort<BaseType>::HandleActivate()){
        return thread_manager_->Activate();
    }
    return false;
}

template <class BaseType, class ProtoType>
bool qpid::SubscriberPort<BaseType, ProtoType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandlePassivate()){
        return TerminateThread();
    }
    return false;
};

template <class BaseType, class ProtoType>
bool qpid::SubscriberPort<BaseType, ProtoType>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandleTerminate()){
        TerminateThread();
        if(thread_manager_){
            delete thread_manager_;
            thread_manager_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseType, class ProtoType>
bool qpid::SubscriberPort<BaseType, ProtoType>::TerminateThread(){
    if(thread_manager_){
        if(thread_manager_->GetState() == ThreadManager::State::ACTIVE){
            try{
                std::lock_guard<std::mutex> lock(connection_mutex_);
                if(connection_.isOpen()){
                    connection_.close();
                }
                return thread_manager_->Terminate();
            }catch(const std::exception& ex){
                Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to close qpid receiver.") + ex.what());
            }
        }
        else{
            return thread_manager_->Terminate();
        }
    }
    return false;
}

template <class BaseType, class ProtoType>
void qpid::SubscriberHandler<BaseType, ProtoType>::Loop(ThreadManager& thread_manager, qpid::SubscriberPort<BaseType, ProtoType>& port){
    bool success = true;
    try{
        std::lock_guard<std::mutex> lock(port.connection_mutex_);
        //Construct a Qpid Connection
        port.connection_ = qpid::messaging::Connection(port.broker_->String());
        //Open the connection
        port.connection_.open();
        auto session = port.connection_.createSession();
        port.receiver_ = session.createReceiver( "amq.topic/pubsub/"  + port.topic_name_->String());
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(std::string("Unable to startup QPID AMQP Reciever") + ex.what());
    }

    //Sleep for 250 ms
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if(success){
        thread_manager.Thread_Configured();

        if(thread_manager.Thread_WaitForActivate()){
            thread_manager.Thread_Activated();
            port.LogActivation();
            bool run = true;
            while(run){
                try{
                    auto sample = port.receiver_.fetch();
                    if(port.receiver_.isClosed()){
                        run = false;
                    }
                    std::string str = sample.getContent();
                    auto base_request_ptr = ::Proto::Translator<BaseType, ProtoType>::StringToBase(str);
                    port.EnqueueMessage(base_request_ptr);
                }
                catch(const std::exception& ex){
                    Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(std::string("Unable to fetch QPID Messages") + ex.what());
                    break;
                }
            }
            port.LogPassivation();
        }
    }
    thread_manager.Thread_Terminated();
};

#endif //QPID_PORT_SUBSCRIBER_HPP
