#ifndef QPID_PORT_REPLIER_HPP
#define QPID_PORT_REPLIER_HPP

#include <core/threadmanager.h>
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
            ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper);
            ~ReplierPort(){
                Activatable::Terminate();
            };
            using middleware_reply_type = ProtoReplyType;
            using middleware_request_type = ProtoRequestType;
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
            bool HandleActivate();
        private:
            bool TerminateThread();
            
            std::mutex mutex_;
            std::unique_ptr<ThreadManager> thread_manager_;
            std::unique_ptr<PortHelper> port_helper_;
            
            std::shared_ptr<Attribute> topic_name_;
            std::shared_ptr<Attribute> broker_;
    };

    //Generic templated RequesterHandler
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    struct RequestHandler{
        static void Loop(ThreadManager& thread_manager, qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port, PortHelper& port_helper, const std::string& topic);
    };
    
    //Specialised templated RequesterHandler for void returning
    template <class BaseRequestType, class ProtoRequestType>
    struct RequestHandler<void, void, BaseRequestType, ProtoRequestType>{
        static void Loop(ThreadManager& thread_manager, qpid::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port, PortHelper& port_helper, const std::string& topic);
    };

    //Specialised templated RequesterHandler for void returning
    template <class BaseReplyType, class ProtoReplyType>
    struct RequestHandler<BaseReplyType, ProtoReplyType, void, void>{
        static void Loop(ThreadManager& thread_manager, qpid::ReplierPort<BaseReplyType, ProtoReplyType, void, void>& port, PortHelper& port_helper, const std::string& topic);
    };
};

//Generic templated ReplierPort
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper):
::ReplierPort<BaseReplyType, BaseRequestType>(component, port_name, callback_wrapper, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_)
        throw std::runtime_error("qpid Replier Port: '" + this->get_name() + "': Has an errant ThreadManager!");
    if(port_helper_)
        throw std::runtime_error("qpid Replier Port: '" + this->get_name() + "': Has an errant PortHelper!");
    
    thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
    port_helper_ = std::unique_ptr<PortHelper>(new PortHelper(broker_->String()));

    auto future = std::async(std::launch::async, zmq::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop, std::ref(*thread_manager_), std::ref(*this), std::ref(*port_helper_));
    thread_manager_->SetFuture(std::move(future));
    thread_manager_->Configure();
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleConfigure();
}

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!thread_manager_)
        throw std::runtime_error("qpid ReplierPort Port: '" + this->get_name() + "': Has no ThreadManager!");

    thread_manager_->Activate();
    //Wait for our recv thjread to be up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleActivate();
}

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandlePassivate(){
    InterruptLoop();
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate();
};


template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(mutex_);
    thread_manager_.reset();
    port_helper_.reset();

    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleTerminate();
};


template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::InterruptLoop(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_){
        thread_manager_->SetTerminate();
    }
    if(port_helper_){
        port_helper_->Terminate();
    }
};

//Generic templated RequestHandler
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void qpid::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop(ThreadManager& thread_manager, qpid::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port, PortHelper& port_helper, const std::string& topic_name){
    bool success = true;
    qpid::messaging::Receiver receiver = 0;
    
    try{
        receiver = port_helper.GetReceiver(topic_name);
    }catch(const std::exception & e){
        std::cerr << e.what() << std::endl;
        success = false;
    }

    if(success){
        thread_manager.Thread_Configured();
        if(thread_manager.Thread_WaitForActivate()){
            port.LogActivation();
            thread_manager.Thread_Activated();
            try{
                while(true){
                    auto request = receiver.fetch();
                    const auto& address = request.getReplyTo();

                    if(address){
                        const auto& request_str = request.getContent();
                        auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(request_str);
                        //Call through the base ProcessRequest function, which calls any attached callback
                        auto base_reply = port.ProcessRequest(*base_request_ptr);
                        auto reply_str = ::Proto::Translator<BaseReplyType, ProtoReplyType>::BaseToString(base_reply);
                        
                        auto reply_sender = port_helper.GetSender(address);
                        reply_sender.send(qpid::messaging::Message(reply_str));
                        reply_sender.getSession().acknowledge();
                    }
                }
            }catch(const std::exception& ex){
                Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                break;
            }
            port.LogPassivation();
        }
    }
    thread_manager.Thread_Terminated();
};


//Specialised templated RequestHandler for void returning
template <class BaseRequestType, class ProtoRequestType>
void qpid::RequestHandler<void, void, BaseRequestType, ProtoRequestType>::Loop(ThreadManager& thread_manager, qpid::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port, PortHelper& port_helper, const std::string& topic_name){
    bool success = true;
    qpid::messaging::Receiver receiver = 0;
    
    try{
        receiver = port_helper.GetReceiver(topic_name);
    }catch(const std::exception & e){
        std::cerr << e.what() << std::endl;
        success = false;
    }

    if(success){
        thread_manager.Thread_Configured();
        if(thread_manager.Thread_WaitForActivate()){
            port.LogActivation();
            thread_manager.Thread_Activated();
            try{
                while(true){
                    auto request = receiver.fetch();
                    const auto& address = request.getReplyTo();

                    if(address){
                        const auto& request_str = request.getContent();
                        auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(request_str);
                        //Call through the base ProcessRequest function, which calls any attached callback
                        port.ProcessRequest(*base_request_ptr);
                        
                        auto reply_sender = port_helper.GetSender(address);
                        reply_sender.send(qpid::messaging::Message());
                        reply_sender.getSession().acknowledge();
                    }
                }
            }catch(const std::exception& ex){
                Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                break;
            }
            port.LogPassivation();
        }
    }
    thread_manager.Thread_Terminated();
};


//Specialised templated RequestHandler for void requesting
template <class BaseReplyType, class ProtoReplyType>
void qpid::RequestHandler<BaseReplyType, ProtoReplyType, void, void>::Loop(ThreadManager& thread_manager, qpid::ReplierPort<BaseReplyType, ProtoReplyType, void, void>& port, PortHelper& port_helper, const std::string& topic_name){
    bool success = true;
    qpid::messaging::Receiver receiver = 0;
    
    try{
        receiver = port_helper.GetReceiver(topic_name);
    }catch(const std::exception & e){
        std::cerr << e.what() << std::endl;
        success = false;
    }

    if(success){
        thread_manager.Thread_Configured();
        if(thread_manager.Thread_WaitForActivate()){
            port.LogActivation();
            thread_manager.Thread_Activated();
            try{
                while(true){
                    auto request = receiver.fetch();
                    const auto& address = request.getReplyTo();

                    if(address){
                        //Call through the base ProcessRequest function, which calls any attached callback
                        auto base_reply = port.ProcessRequest();
                        auto reply_str = ::Proto::Translator<BaseReplyType, ProtoReplyType>::BaseToString(base_reply);
                        
                        auto reply_sender = port_helper.GetSender(address);
                        reply_sender.send(qpid::messaging::Message(reply_str));
                        reply_sender.getSession().acknowledge();
                    }
                }
            }catch(const std::exception& ex){
                Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                break;
            }
            port.LogPassivation();
        }
    }
    thread_manager.Thread_Terminated();
};

#endif //QPID_PORT_REPLIER_HPP