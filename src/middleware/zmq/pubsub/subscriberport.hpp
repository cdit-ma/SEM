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
            void HandleActivate();
            void HandleConfigure();
            void HandlePassivate();
            void HandleTerminate();
        private:
            void InterruptLoop();
            void Loop(ThreadManager& thread_manager, const std::string& terminate_address, const std::vector<std::string> connect_addresses);
            
            std::mutex thread_manager_mutex_;
            std::unique_ptr<ThreadManager> thread_manager_;

            std::shared_ptr<Attribute> end_points_;

            std::string terminate_endpoint_;
            const std::string terminate_str = "TERMINATE";
    }; 
};



template <class BaseType, class ProtoType>
zmq::SubscriberPort<BaseType, ProtoType>::SubscriberPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<void, BaseType>& callback_wrapper):
::SubscriberPort<BaseType>(component, port_name, callback_wrapper, "zmq"){
    auto component_ = component.lock();
    auto component_name = component_ ? component_->get_name() : "??";
    auto component_id = component_ ? component_->get_id() : "??";
    auto port_id = ::Activatable::get_id();
    terminate_endpoint_ = "inproc://term*" + component_name + "*" + component_id + "*" + port_name + "*" + port_id;
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
};

template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(!thread_manager_){
        thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
        auto future = std::async(std::launch::async, &zmq::SubscriberPort<BaseType, ProtoType>::Loop, this, std::ref(*thread_manager_), terminate_endpoint_, end_points_->get_StringList());
        thread_manager_->SetFuture(std::move(future));
        thread_manager_->Configure();
    }else{
        throw std::runtime_error("zmq::SubscriberPort has an active ThreadManager");
    }
    ::SubscriberPort<BaseType>::HandleConfigure();
}

template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_){
        thread_manager_->Activate();
    }else{
        throw std::runtime_error("zmq::SubscriberPort has no Thread Manager");
    }
    ::SubscriberPort<BaseType>::HandleActivate();
    this->logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::ACTIVATED);
};

template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::HandlePassivate(){
    InterruptLoop();
    ::SubscriberPort<BaseType>::HandlePassivate();
    this->logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::PASSIVATED);
}


template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_){
        thread_manager_->Terminate();
        thread_manager_.reset();
    }
    ::SubscriberPort<BaseType>::HandleTerminate();
    this->logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::TERMINATED);
};


template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::InterruptLoop(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_){
        try{
            auto socket = ZmqHelper::get_zmq_helper().get_publisher_socket();
            //Connect to the terminate address
            socket->bind(terminate_endpoint_.c_str());
            
            //XXX: DO NOT REMOVE THE SLEEP ZMQ NEEDS HIS REST
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            //Send the special terminate message
            zmq::message_t term_msg(terminate_str.c_str(), terminate_str.size());
            socket->send(term_msg);
        }catch(const zmq::error_t& ex){
            Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to Terminate ZMQ Server Port: '") + ex.what() + "'");
        }
    }
};


template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::Loop(ThreadManager& thread_manager, const std::string& terminate_address, const std::vector<std::string> connect_addresses){
    auto& helper = zmq::ZmqHelper::get_zmq_helper();
    auto socket = helper.get_subscriber_socket();

    bool success = true;

    try{
        socket->connect(terminate_address.c_str());
    }catch(zmq::error_t ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to terminate endpoint: '" + terminate_endpoint_ + "' " + ex.what());
        success = false;
    }

    for(const auto& endpoint : connect_addresses){
        try{
            //connect the addresses provided
            socket->connect(endpoint.c_str());
        }catch(zmq::error_t ex){
            Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to endpoint: '" + endpoint + "' " + ex.what());
            success = false;
        }
    }

    if(success){
        //XXX: DO NOT REMOVE THE SLEEP ZMQ NEEDS HIS REST
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        thread_manager.Thread_Configured();

        if(thread_manager.Thread_WaitForActivate()){
            ::Port::LogActivation();
            thread_manager.Thread_Activated();
            while(true){
                try{
                    //Wait for next message
                    zmq::message_t zmq_request;
                    socket->recv(&zmq_request);
                    const auto& request_str = Zmq2String(zmq_request);
                
                    if(request_str == terminate_str){
                        break;
                    }

                    auto BaseType_ptr = std::unique_ptr<BaseType>(::Proto::Translator<BaseType, ProtoType>::StringToBase(request_str));
                    this->EnqueueMessage(std::move(BaseType_ptr));
                }catch(zmq::error_t ex){
                    Log(Severity::ERROR_).Context(this).Func(__func__).Msg(ex.what());
                    break;
                }
            }
            ::Port::LogPassivation();
        }
    }
    thread_manager.Thread_Terminated();
};


#endif // ZMQ_PORT_SUBSCRIBER_HPP
