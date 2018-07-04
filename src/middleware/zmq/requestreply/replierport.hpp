#ifndef ZMQ_PORT_REPLIER_HPP
#define ZMQ_PORT_REPLIER_HPP

#include <core/threadmanager.h>
#include <core/ports/requestreply/replierport.hpp>
#include <middleware/proto/prototranslator.h>
#include <middleware/zmq/zmqhelper.h>
#include <re_common/zmq/zmqutils.hpp>

namespace zmq{
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    struct RequestHandler;

    //Generic templated ReplierPort
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    class ReplierPort : public ::ReplierPort<BaseReplyType, BaseRequestType>{
        //The Request Handle needs to be able to modify and change state of the Port
        friend class RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>;
        
        public:
            ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper);
            ~ReplierPort(){this->Terminate();};

            using middleware_reply_type = ProtoReplyType;
            using middleware_request_type = ProtoRequestType;
        protected:
            void HandleActivate();
            void HandleConfigure();
            void HandlePassivate();
            void HandleTerminate();
        private:
            void InterruptLoop();

            std::mutex thread_manager_mutex_;
            std::unique_ptr<ThreadManager> thread_manager_;

            std::shared_ptr<Attribute> server_address_;
            std::string terminate_endpoint_;
            const std::string terminate_str = "TERMINATE";
    };

    //Generic templated RequesterHandler
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    struct RequestHandler{
        static void Loop(ThreadManager& thread_manager, zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port, const std::string terminate_address, const std::string server_address);
    };
    
    //Specialised templated RequesterHandler for void returning
    template <class BaseRequestType, class ProtoRequestType>
    struct RequestHandler<void, void, BaseRequestType, ProtoRequestType>{
        static void Loop(ThreadManager& thread_manager, zmq::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port, const std::string terminate_address, const std::string server_address);
    };

    //Specialised templated RequesterHandler for void requesting
    template <class BaseReplyType, class ProtoReplyType>
    struct RequestHandler<BaseReplyType, ProtoReplyType, void, void>{
        static void Loop(ThreadManager& thread_manager, zmq::ReplierPort<BaseReplyType, ProtoReplyType, void, void>& port, const std::string terminate_address, const std::string server_address);
    };
};

//Generic templated ReplierPort
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name,  const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper):
::ReplierPort<BaseReplyType, BaseRequestType>(component, port_name, callback_wrapper, "zmq"){
    auto component_ = component.lock();
    auto component_name = component_ ? component_->get_name() : "??";
    auto component_id = component_ ? component_->get_id() : "??";
    auto port_id = ::Activatable::get_id();
    terminate_endpoint_ = "inproc://term*" + component_name + "*" + component_id + "*" + port_name + "*" + port_id;

    server_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(!thread_manager_){
        thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
        auto future = std::async(std::launch::async, zmq::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop, std::ref(*thread_manager_), std::ref(*this), terminate_endpoint_, server_address_->get_String());
        thread_manager_->SetFuture(std::move(future));
        thread_manager_->Configure();
    }else{
        throw std::runtime_error("zmq::ReplierPort has an active ThreadManager");
    }
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleConfigure();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_){
        thread_manager_->Activate();
    }else{
        throw std::runtime_error("zmq::ReplierPort has no Thread Manager");
    }
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleActivate();
    this->logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::ACTIVATED);
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandlePassivate(){
    InterruptLoop();
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate();
    this->logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::PASSIVATED);
};



template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_){
        thread_manager_->Terminate();
        thread_manager_.reset();
    }
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleTerminate();
    this->logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::TERMINATED);
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::InterruptLoop(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_ && thread_manager_->isKillable()){
        try{
            auto socket = ZmqHelper::get_zmq_helper().get_request_socket();
            //Connect to the terminate address
            socket->connect(terminate_endpoint_.c_str());
                
            //Send the special terminate message
            zmq::message_t term_msg(terminate_str.c_str(), terminate_str.size());
            socket->send(term_msg);
            //Wait for the response
            socket->recv(&term_msg);
        }catch(const zmq::error_t& ex){
            Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to Terminate ZMQ Server Port") + ex.what());
        }
    }
}


//Generic templated RequestHandler
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop(ThreadManager& thread_manager, zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port, const std::string terminate_address, const std::string server_address){
    auto socket = ZmqHelper::get_zmq_helper().get_reply_socket();
    bool success = true;
    try{
        //Bind the Terminate address
        socket->bind(terminate_address.c_str());
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind terminate endpoint: '" + terminate_address + "' " + ex.what());
        success = false;
    }

    try{
        //Bind the Outgoing address
        socket->bind(server_address.c_str());
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind endpoint: '" + server_address + "' " + ex.what());
        success = false;
    }

    if(success){
        //XXX: DO NOT REMOVE THE SLEEP ZMQ NEEDS HIS REST
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        thread_manager.Thread_Configured();

        if(thread_manager.Thread_WaitForActivate()){
            port.LogActivation();
            thread_manager.Thread_Activated();
            while(true){
                try{
                    //Wait for next message
                    zmq::message_t zmq_request;
                    socket->recv(&zmq_request);

                    const auto& request_str = Zmq2String(zmq_request);
                    
                    if(request_str == port.terminate_str){
                        //Send back a Terminate
                        socket->send(zmq_request);
                        break;
                    }
                    
                    auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(request_str);
                    
                    //Call through the base ProcessRequest function, which calls any attached callback
                    auto base_reply = port.ProcessRequest(*base_request_ptr);
                    auto reply_str = ::Proto::Translator<BaseReplyType, ProtoReplyType>::BaseToString(base_reply);
                    
                    //Send reply
                    socket->send(String2Zmq(reply_str));
                }catch(const zmq::error_t& ex){
                    Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                    break;
                }
            }
            //Log that the port has been passivated
            port.LogPassivation();
        }
    }
    thread_manager.Thread_Terminated();
};

//Specialised templated RequestHandler for void returning
template <class BaseRequestType, class ProtoRequestType>
void zmq::RequestHandler<void, void, BaseRequestType, ProtoRequestType>::Loop(ThreadManager& thread_manager, zmq::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port, const std::string terminate_address, const std::string server_address){
    auto socket = ZmqHelper::get_zmq_helper().get_reply_socket();
    bool success = true;
    try{
        //Bind the Terminate address
        socket->bind(terminate_address.c_str());
    }catch(const zmq::error_t& ex){
        std::cerr << "ERR" << std::endl;
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind terminate endpoint: '" + terminate_address + "' " + ex.what());
        success = false;
    }

    try{
        //Bind the Outgoing address
        socket->bind(server_address.c_str());
    }catch(const zmq::error_t& ex){
        std::cerr << "ERR" << std::endl;
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind endpoint: '" + server_address + "' " + ex.what());
        success = false;
    }

    if(success){
        //XXX: DO NOT REMOVE THE SLEEP ZMQ NEEDS HIS REST
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        thread_manager.Thread_Configured();

        if(thread_manager.Thread_WaitForActivate()){
            thread_manager.Thread_Activated();
            while(true){
                try{
                    //Wait for next message
                    zmq::message_t zmq_request;
                    socket->recv(&zmq_request);

                    const auto& request_str = Zmq2String(zmq_request);
                    
                    if(request_str == port.terminate_str){
                        //Send back a Terminate
                        socket->send(zmq_request);
                        break;
                    }
                    
                    auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(request_str);
                    
                    //Call through the base ProcessRequest function, which calls any attached callback
                    port.ProcessRequest(*base_request_ptr);
                    //Send reply
                    socket->send(String2Zmq(""));
                }catch(const zmq::error_t& ex){
                    Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                    break;
                }
            }
            //Log that the port has been passivated
            port.LogPassivation();
        }
    }
    thread_manager.Thread_Terminated();
};



//Specialised templated RequestHandler for void returning
template <class BaseReplyType, class ProtoReplyType>
void zmq::RequestHandler<BaseReplyType, ProtoReplyType, void, void>::Loop(ThreadManager& thread_manager, zmq::ReplierPort<BaseReplyType, ProtoReplyType, void, void>& port, const std::string terminate_address, const std::string server_address){
    auto socket = ZmqHelper::get_zmq_helper().get_reply_socket();
    bool success = true;
    try{
        //Bind the Terminate address
        socket->bind(terminate_address.c_str());
    }catch(const zmq::error_t& ex){
        std::cerr << "ERR" << std::endl;
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind terminate endpoint: '" + terminate_address + "' " + ex.what());
        success = false;
    }

    try{
        //Bind the Outgoing address
        socket->bind(server_address.c_str());
    }catch(const zmq::error_t& ex){
        std::cerr << "ERR" << std::endl;
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind endpoint: '" + server_address + "' " + ex.what());
        success = false;
    }

    if(success){
        //XXX: DO NOT REMOVE THE SLEEP ZMQ NEEDS HIS REST
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        thread_manager.Thread_Configured();

        if(thread_manager.Thread_WaitForActivate()){
            thread_manager.Thread_Activated();
            while(true){
                try{
                    //Wait for next message
                    zmq::message_t zmq_request;
                    socket->recv(&zmq_request);

                    const auto& request_str = Zmq2String(zmq_request);
                    
                    if(request_str == port.terminate_str){
                        //Send back a Terminate
                        socket->send(zmq_request);
                        break;
                    }
                    
                    //Call through the base ProcessRequest function, which calls any attached callback
                    auto base_reply = port.ProcessRequest();
                    auto reply_str = ::Proto::Translator<BaseReplyType, ProtoReplyType>::BaseToString(base_reply);
                    
                    //Send reply
                    socket->send(String2Zmq(reply_str));
                }catch(const zmq::error_t& ex){
                    Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                    break;
                }
            }
            //Log that the port has been passivated
            port.LogPassivation();
        }
    }
    thread_manager.Thread_Terminated();
}



#endif //ZMQ_PORT_REPLIER_HPP
