#ifndef ZMQ_PORT_SUBSCRIBER_HPP
#define ZMQ_PORT_SUBSCRIBER_HPP

#include <core/threadmanager.h>
#include <core/ports/pubsub/subscriberport.hpp>
#include <middleware/proto/prototranslator.h>
#include <middleware/zmq/zmqhelper.h>
#include <re_common/zmq/zmqutils.hpp>

namespace zmq{
    template <class BaseType, class ProtoType>
    class SubscriberPort : public ::SubscriberPort<BaseType>{
        public:
            SubscriberPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<void, BaseType>& callback_wrapper);
            ~SubscriberPort(){this->Terminate();};
            using middleware_type = ProtoType;
        protected:
            void HandleConfigure();
            void HandleActivate();
            void HandlePassivate();
            void HandleTerminate();
        private:
            void InterruptLoop();
            void Loop(ThreadManager& thread_manager, std::unique_ptr<zmq::socket_t> socket);
            
            std::mutex mutex_;
            std::unique_ptr<ThreadManager> thread_manager_;
            std::unique_ptr<zmq::context_t> context_;

            std::shared_ptr<Attribute> end_points_;
    }; 
};



template <class BaseType, class ProtoType>
zmq::SubscriberPort<BaseType, ProtoType>::SubscriberPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<void, BaseType>& callback_wrapper):
::SubscriberPort<BaseType>(component, port_name, callback_wrapper, "zmq"){
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_addresses").lock();
};

template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_)
        throw std::runtime_error("ZMQ Subscriber Port: '" + this->get_name() + "': Has an errant ThreadManager!");
    if(context_)
        throw std::runtime_error("ZMQ Subscriber Port: '" + this->get_name() + "': Has an errant ZMQ Context!");

    auto& helper = get_zmq_helper();
    context_ = helper.get_context();

    //Get a subscriber port
    auto socket = helper.get_socket(*context_, ZMQ_SUB);
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);

    for(const auto& endpoint : end_points_->StringList()){
        try{
            socket->connect(endpoint);
        }catch(const zmq::error_t& ex){
            throw std::runtime_error("ZMQ Subscriber Port: '" + this->get_name() + "': Failed to connect to: '" + endpoint);
        }
    }

    thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
    auto future = std::async(std::launch::async, &zmq::SubscriberPort<BaseType, ProtoType>::Loop, this, std::ref(*thread_manager_), std::move(socket));
    thread_manager_->SetFuture(std::move(future));
    thread_manager_->Configure();
    ::SubscriberPort<BaseType>::HandleConfigure();
}

template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!thread_manager_)
        throw std::runtime_error("ZMQ Subscriber Port: '" + this->get_name() + "': Has no ThreadManager!");

    thread_manager_->Activate();
    //Wait for our recv thjread to be up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));


    ::SubscriberPort<BaseType>::HandleActivate();
};

template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::HandlePassivate(){
    InterruptLoop();
    ::SubscriberPort<BaseType>::HandlePassivate();
}


template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(mutex_);
    thread_manager_.reset();
    
    ::SubscriberPort<BaseType>::HandleTerminate();
};


template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::InterruptLoop(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_){
        thread_manager_->SetTerminate();
    }
    //Free the context, which will block until all sockets are released
    context_.reset();
};


template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::Loop(ThreadManager& thread_manager, std::unique_ptr<socket_t> socket){
    thread_manager.Thread_Configured();

    if(thread_manager.Thread_WaitForActivate()){
        thread_manager.Thread_Activated();
        try{
            while(true){
                //Wait for next message
                zmq::message_t zmq_request;
                socket->recv(&zmq_request);
                const auto& request_str = Zmq2String(zmq_request);
                try{
                    auto basetype_ptr = std::unique_ptr<BaseType>(::Proto::Translator<BaseType, ProtoType>::StringToBase(request_str));
                    this->EnqueueMessage(std::move(basetype_ptr));
                }catch(const std::exception& ex){
                    std::string error_str("Failed to translate subscribed message: ");
                    this->ProcessGeneralException(error_str + ex.what(), true);
                }
            }
        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                this->ProcessGeneralException(ex.what(), true);
            }
        }
    }
    thread_manager.Thread_Terminated();
};


#endif // ZMQ_PORT_SUBSCRIBER_HPP
