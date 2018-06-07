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
            SubscriberPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<void (BaseType&) > callback_function);
            ~SubscriberPort(){
                Activatable::Terminate();
            };
            using middleware_type = ProtoType;
        protected:
            bool HandleActivate();
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            void Loop(ThreadManager& thread_manager, const std::string& terminate_address, const std::vector<std::string> connect_addresses);
            bool TerminateThread();
            std::mutex control_mutex_;

            ThreadManager* thread_manager_ = 0;

            std::shared_ptr<Attribute> end_points_;

            std::string terminate_endpoint_;
            const std::string terminate_str = "TERMINATE";
    }; 
};



template <class BaseType, class ProtoType>
zmq::SubscriberPort<BaseType, ProtoType>::SubscriberPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<void (BaseType&) > callback_function):
::SubscriberPort<BaseType>(component, port_name, callback_function, "zmq"){
    auto component_ = component.lock();
    auto component_name = component_ ? component_->get_name() : "??";
    auto component_id = component_ ? component_->get_id() : "??";
    auto port_id = ::Activatable::get_id();
    terminate_endpoint_ = "inproc://term*" + component_name + "*" + component_id + "*" + port_name + "*" + port_id;
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
};

template <class BaseType, class ProtoType>
bool zmq::SubscriberPort<BaseType, ProtoType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    const auto end_points = end_points_->StringList();
    bool valid = end_points.size() > 0;

    if(valid && ::SubscriberPort<BaseType>::HandleConfigure()){
        if(!thread_manager_){
            thread_manager_ = new ThreadManager();
            auto thread = std::unique_ptr<std::thread>(new std::thread(&zmq::SubscriberPort<BaseType, ProtoType>::Loop, this, std::ref(*thread_manager_), terminate_endpoint_, end_points));
            thread_manager_->SetThread(std::move(thread));
            return thread_manager_->Configure();
        }else{
            std::cerr << "STILL GOT THREAD" << std::endl;
        }
    }
    return false;
};

template <class BaseType, class ProtoType>
bool zmq::SubscriberPort<BaseType, ProtoType>::HandleActivate(){
    //std::cerr << "HandleActivate" << std::endl;
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    if(::SubscriberPort<BaseType>::HandleActivate()){
        return thread_manager_->Activate();
    }
    return false;
};


template <class BaseType, class ProtoType>
bool zmq::SubscriberPort<BaseType, ProtoType>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandleTerminate()){
        //Terminate the thread
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
bool zmq::SubscriberPort<BaseType, ProtoType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandlePassivate()){
        return TerminateThread();
    }
    return false;
};


template <class BaseType, class ProtoType>
bool zmq::SubscriberPort<BaseType, ProtoType>::TerminateThread(){
    if(thread_manager_){
        if(thread_manager_->GetState() == ThreadManager::State::ACTIVE){
            try{
                auto socket = ZmqHelper::get_zmq_helper()->get_publisher_socket();
            
                //Connect to the terminate address
                socket.bind(terminate_endpoint_.c_str());

                //XXX: DO NOT REMOVE THE SLEEP ZMQ NEEDS HIS REST
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                //Send the special terminate message
                zmq::message_t term_msg(terminate_str.c_str(), terminate_str.size());
                
                socket.send(term_msg);
            }catch(const zmq::error_t& ex){
                Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to Terminate ZMQ Server Port") + ex.what());
            }
        }
        return thread_manager_->Terminate();
    }
    return true;
};


template <class BaseType, class ProtoType>
void zmq::SubscriberPort<BaseType, ProtoType>::Loop(ThreadManager& thread_manager, const std::string& terminate_address, const std::vector<std::string> connect_addresses){
    auto helper = zmq::ZmqHelper::get_zmq_helper();
    auto socket = helper->get_subscriber_socket();

    bool success = true;

    try{
        socket.connect(terminate_address.c_str());
    }catch(zmq::error_t ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to terminate endpoint: '" + terminate_endpoint_ + "' " + ex.what());
        success = false;
    }

    for(const auto& endpoint : connect_addresses){
        try{
            //connect the addresses provided
            socket.connect(endpoint.c_str());
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
                    socket.recv(&zmq_request);
                    const auto& request_str = Zmq2String(zmq_request);
                
                    if(request_str == terminate_str){
                        break;
                    }
                    auto base_request_ptr = ::Proto::Translator<BaseType, ProtoType>::StringToBase(request_str);
                    this->EnqueueMessage(base_request_ptr);
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
