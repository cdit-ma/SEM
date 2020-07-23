#ifndef QPID_PORT_SUBSCRIBER_HPP
#define QPID_PORT_SUBSCRIBER_HPP

#include "threadmanager.h"
#include "prototranslator.hpp"
#include "ports/pubsub/subscriberport.hpp"
#include "qpidhelper.h"

#include <iostream>
#include <mutex>
#include <string>
#include <functional>



namespace qpid{
    template <class BaseType, class ProtoType>
    struct SubscriberHandler;

    template <class BaseType, class ProtoType> class SubscriberPort: public ::SubscriberPort<BaseType>{
        friend class SubscriberHandler<BaseType, ProtoType>;
        public:
            SubscriberPort(std::weak_ptr<Component> component, std::string name, const CallbackWrapper<void, BaseType>& callback_wrapper);
            ~SubscriberPort(){this->Terminate();};
            using middleware_type = ProtoType;
        protected:
            void HandleConfigure();
            void HandlePassivate();
            void HandleTerminate();
            void HandleActivate();
        private:
            void InterruptLoop();
            void Loop(ThreadManager& thread_manager, PortHelper& port_helper, const std::string& topic_name);

            ::Proto::Translator<BaseType, ProtoType> translator;

            std::mutex mutex_;
            std::unique_ptr<ThreadManager> thread_manager_;
            std::unique_ptr<PortHelper> port_helper_;

            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;
    };
};

template <class BaseType, class ProtoType>
qpid::SubscriberPort<BaseType, ProtoType>::SubscriberPort(std::weak_ptr<Component> component, std::string name, const CallbackWrapper<void, BaseType>& callback_wrapper):
::SubscriberPort<BaseType>(component, name, callback_wrapper, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseType, class ProtoType>
void qpid::SubscriberPort<BaseType, ProtoType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(port_helper_)
        throw std::runtime_error("qpid Subscriber Port: '" + this->get_name() + "': Has an errant Port Helper!");
    if(thread_manager_)
        throw std::runtime_error("qpid Subscriber Port: '" + this->get_name() + "': Has an errant Thread Manager!");

    
    port_helper_ =  std::unique_ptr<PortHelper>(new PortHelper(broker_->String()));
    thread_manager_ =  std::unique_ptr<ThreadManager>(new ThreadManager());
    auto future = std::async(std::launch::async, &qpid::SubscriberPort<BaseType, ProtoType>::Loop, this, std::ref(*thread_manager_), std::ref(*port_helper_), topic_name_->String());
    thread_manager_->SetFuture(std::move(future));
    thread_manager_->WaitForConfigured();
    ::SubscriberPort<BaseType>::HandleConfigure();
};

template <class BaseType, class ProtoType>
void qpid::SubscriberPort<BaseType, ProtoType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!thread_manager_)
        throw std::runtime_error("qpid Subscriber Port: '" + this->get_name() + "': Has no ThreadManager!");

    thread_manager_->Activate();
    //Wait for our recv thjread to be up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::SubscriberPort<BaseType>::HandleActivate();
}

template <class BaseType, class ProtoType>
void qpid::SubscriberPort<BaseType, ProtoType>::HandlePassivate(){
    InterruptLoop();
    ::SubscriberPort<BaseType>::HandlePassivate();
};

template <class BaseType, class ProtoType>
void qpid::SubscriberPort<BaseType, ProtoType>::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(mutex_);
    thread_manager_.reset();
    port_helper_.reset();

    ::SubscriberPort<BaseType>::HandleTerminate();
};

template <class BaseType, class ProtoType>
void qpid::SubscriberPort<BaseType, ProtoType>::InterruptLoop(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_){
        thread_manager_->SetTerminate();
    }
    if(port_helper_){
        port_helper_->Terminate();
    }
}

template <class BaseType, class ProtoType>
void qpid::SubscriberPort<BaseType, ProtoType>::Loop(ThreadManager& thread_manager, PortHelper& port_helper, const std::string& topic_name){
    bool success = true;
    qpid::messaging::Receiver receiver = 0;
    
    try{
        receiver = port_helper.GetReceiver(topic_name);
    }catch(const std::exception& ex){
        std::cerr << ex.what() << std::endl;
        success = false;
    }

    if(success){
        thread_manager.Thread_Configured();
        if(thread_manager.Thread_WaitForActivate()){
            thread_manager.Thread_Activated();
            try{
                while(true){
                    auto request = receiver.fetch();
                    const auto& request_str = request.getContent();

                    try{
                        auto basetype_ptr = ::Proto::Translator<BaseType, ProtoType>::StringToBase(request_str);
                        this->EnqueueMessage(std::move(basetype_ptr));
                    }catch(const std::exception& ex){
                        std::string error_str("Failed to translate subscribed message: ");;
                        this->ProcessGeneralException(error_str + ex.what());
                    }
                }
            }catch(const std::exception& ex){
                this->ProcessGeneralException(ex.what());
            }
        }
    }

    thread_manager.Thread_Terminated();
};

#endif //QPID_PORT_SUBSCRIBER_HPP
