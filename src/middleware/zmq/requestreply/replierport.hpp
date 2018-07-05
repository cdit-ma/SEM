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

            std::mutex mutex_;
            std::unique_ptr<ThreadManager> thread_manager_;
            std::unique_ptr<zmq::context_t> context_;

            std::shared_ptr<Attribute> server_address_;
    };

    //Generic templated RequesterHandler
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    struct RequestHandler{
        static void Loop(ThreadManager& thread_manager, zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port, std::unique_ptr<zmq::socket_t> socket);
    };
    
    //Specialised templated RequesterHandler for void returning
    template <class BaseRequestType, class ProtoRequestType>
    struct RequestHandler<void, void, BaseRequestType, ProtoRequestType>{
        static void Loop(ThreadManager& thread_manager, zmq::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port, std::unique_ptr<zmq::socket_t> socket);
    };

    //Specialised templated RequesterHandler for void requesting
    template <class BaseReplyType, class ProtoReplyType>
    struct RequestHandler<BaseReplyType, ProtoReplyType, void, void>{
        static void Loop(ThreadManager& thread_manager, zmq::ReplierPort<BaseReplyType, ProtoReplyType, void, void>& port, std::unique_ptr<zmq::socket_t> socket);
    };
};

//Generic templated ReplierPort
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name,  const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper):
::ReplierPort<BaseReplyType, BaseRequestType>(component, port_name, callback_wrapper, "zmq"){
    server_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_)
        throw std::runtime_error("ZMQ Replier Port: '" + this->get_name() + "': Has an errant ThreadManager!");
    if(context_)
        throw std::runtime_error("ZMQ Replier Port: '" + this->get_name() + "': Has an errant ZMQ Context!");

    auto& helper = get_zmq_helper();
    context_ = helper.get_context();

    //Get a subscriber port
    auto socket = helper.get_socket(*context_, ZMQ_REP);
    socket->setsockopt(ZMQ_LINGER, 0);

    const auto& server_endpoint = server_address_->String();
    try{
        socket->bind(server_endpoint);
    }catch(const zmq::error_t& ex){
        throw std::runtime_error("ZMQ Replier Port: '" + this->get_name() + "': Failed to bind: '" + server_endpoint);
    }

    //Wait for our recv thread to be up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));


    thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
    auto future = std::async(std::launch::async, zmq::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop, std::ref(*thread_manager_), std::ref(*this), std::move(socket));
    thread_manager_->SetFuture(std::move(future));
    thread_manager_->Configure();
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleConfigure();
}

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!thread_manager_)
        throw std::runtime_error("ZMQ ReplierPort Port: '" + this->get_name() + "': Has no ThreadManager!");

    thread_manager_->Activate();
    //Wait for our recv thjread to be up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleActivate();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandlePassivate(){
    InterruptLoop();
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(mutex_);
    thread_manager_.reset();

    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleTerminate();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::InterruptLoop(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_){
        thread_manager_->SetTerminate();
    }
    //Free the context, which will block until all sockets are released
    context_.reset();
}


//Generic templated RequestHandler
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop(ThreadManager& thread_manager, zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port, std::unique_ptr<zmq::socket_t> socket){
    thread_manager.Thread_Configured();

    if(thread_manager.Thread_WaitForActivate()){
        thread_manager.Thread_Activated();
        
        try{
            while(true){
                //Wait for next message
                zmq::message_t zmq_request;
                socket->recv(&zmq_request);

                auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(Zmq2String(zmq_request));
                    
                //Call through the base ProcessRequest function, which calls any attached callback
                auto base_reply = port.ProcessRequest(*base_request_ptr);
                auto reply_str = ::Proto::Translator<BaseReplyType, ProtoReplyType>::BaseToString(base_reply);
                delete base_request_ptr;
                //Send reply
                socket->send(String2Zmq(reply_str));
            }
        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                std::cerr << "zmq::RequestHandler: '" + port.get_name() + "' " << ex.what() << std::endl;
            }
        }
    }
    thread_manager.Thread_Terminated();
};

//Specialised templated RequestHandler for void returning
template <class BaseRequestType, class ProtoRequestType>
void zmq::RequestHandler<void, void, BaseRequestType, ProtoRequestType>::Loop(ThreadManager& thread_manager, zmq::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port, std::unique_ptr<zmq::socket_t> socket){
    thread_manager.Thread_Configured();

    if(thread_manager.Thread_WaitForActivate()){
        thread_manager.Thread_Activated();
        
        try{
            while(true){
                //Wait for next message
                zmq::message_t zmq_request;
                socket->recv(&zmq_request);

                auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(Zmq2String(zmq_request));
                    
                //Call through the base ProcessRequest function, which calls any attached callback
                port.ProcessRequest(*base_request_ptr);
                delete base_request_ptr;
                socket->send(zmq::message_t());
            }
        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                std::cerr << "zmq::RequestHandler: '" + port.get_name() + "' " << ex.what() << std::endl;
            }
        }
    }
    thread_manager.Thread_Terminated();
};



//Specialised templated RequestHandler for void returning
template <class BaseReplyType, class ProtoReplyType>
void zmq::RequestHandler<BaseReplyType, ProtoReplyType, void, void>::Loop(ThreadManager& thread_manager, zmq::ReplierPort<BaseReplyType, ProtoReplyType, void, void>& port, std::unique_ptr<zmq::socket_t> socket){
    thread_manager.Thread_Configured();

    if(thread_manager.Thread_WaitForActivate()){
        thread_manager.Thread_Activated();
        
        try{
            while(true){
                //Wait for next message
                zmq::message_t zmq_request;
                socket->recv(&zmq_request);

                //Call through the base ProcessRequest function, which calls any attached callback
                auto base_reply = port.ProcessRequest();
                auto reply_str = ::Proto::Translator<BaseReplyType, ProtoReplyType>::BaseToString(base_reply);
                //Send reply
                socket->send(String2Zmq(reply_str));
            }
        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                std::cerr << "zmq::RequestHandler: '" + port.get_name() + "' " << ex.what() << std::endl;
            }
        }
    }
    thread_manager.Thread_Terminated();
}

#endif //ZMQ_PORT_REPLIER_HPP
