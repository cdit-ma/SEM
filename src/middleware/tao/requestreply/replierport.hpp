#ifndef TAO_PORT_REPLIER_HPP
#define TAO_PORT_REPLIER_HPP

#include <core/threadmanager.h>
#include <core/ports/requestreply/replierport.hpp>
#include <middleware/tao/taohelper.h>

#ifndef TAO_SERVER_FUNC_NAME
//REQUIRES MACRO TAO_SERVER_FUNC_NAME TO BE DEFINED
static_assert(false, "Requires TAO_SERVER_FUNC_NAME To Be Defined")
#endif

//Inspired by
//https://www.codeproject.com/Articles/24863/A-Simple-C-Client-Server-in-CORBA

namespace tao{
    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    struct RequestHandler;

    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    class ReplierPort : public ::ReplierPort<BaseReplyType, BaseRequestType>{
        //The Request Handle needs to be able to modify and change state of the Port
        friend class RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>;
        public:
            ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper);
            ~ReplierPort(){
                Activatable::Terminate();
            };

            using middleware_reply_type = TaoReplyType;
            using middleware_request_type = TaoRequestType;

        protected:
            bool HandleActivate();
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            bool TerminateThread();

            ThreadManager* thread_manager_ = 0;
            
            std::mutex control_mutex_;
            
            std::shared_ptr<Attribute> server_name_;
            std::shared_ptr<Attribute> orb_endpoint_;
    };

    //Generic templated RequesterHandler
    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    struct RequestHandler{
        static void Loop(ThreadManager& thread_manager, tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>& port, const std::string orb_endpoint, const std::string server_name);
    };

    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    class TaoServerImpl : public TaoServerInt{
        public:
            TaoServerImpl(tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>& port):
                eventport(port){
            };
            TaoReplyType* TAO_SERVER_FUNC_NAME(const TaoRequestType& message){
                auto base_message = Base::Translator<BaseRequestType, TaoRequestType>::MiddlewareToBase(message);
                auto base_result = eventport.ProcessRequest(*base_message);
                return Base::Translator<BaseReplyType, TaoReplyType>::BaseToMiddleware(base_result);
            };
        private:
            tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>& eventport;
    };

    template <class BaseRequestType, class TaoRequestType, class TaoServerInt>

    class TaoServerImpl<void, void, BaseRequestType, TaoRequestType, TaoServerInt> : public TaoServerInt{
        public:
            TaoServerImpl(tao::ReplierPort<void, void, BaseRequestType, TaoRequestType, TaoServerInt>& port):
                eventport(port){
            };
            void TAO_SERVER_FUNC_NAME(const TaoRequestType& message){
                auto base_message = Base::Translator<BaseRequestType, TaoRequestType>::MiddlewareToBase(message);
                eventport.ProcessRequest(*base_message);
                delete base_message;
            };
        private:
            tao::ReplierPort<void, void, BaseRequestType, TaoRequestType, TaoServerInt>& eventport;
    };
};

//Generic templated ReplierPort
template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name,  const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper):
::ReplierPort<BaseReplyType, BaseRequestType>(component, port_name, callback_wrapper, "tao"){
    orb_endpoint_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "orb_endpoint").lock();
    server_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_name").lock();
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    const auto orb_endpoint = orb_endpoint_->String();
    const auto server_name = server_name_->String();

    bool valid = orb_endpoint.size() > 0 && server_name.size() > 0;
    if(valid && ::ReplierPort<BaseReplyType, BaseRequestType>::HandleConfigure()){
        if(!thread_manager_){
            thread_manager_ = new ThreadManager();
            auto thread = std::unique_ptr<std::thread>(new std::thread(tao::RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::Loop, std::ref(*thread_manager_), std::ref(*this), orb_endpoint, server_name));
            thread_manager_->SetThread(std::move(thread));
            return thread_manager_->Configure();
        }
    }
    return false;
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::HandleActivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandleActivate()){
        return thread_manager_->Activate();
    }
    return false;
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Call into the base Handle Passivate
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate()){
        return TerminateThread();
    }
    return false;
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    //Call into the base class
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandleTerminate()){
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


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::TerminateThread(){
    if(thread_manager_){
        return thread_manager_->Terminate();
    }
    return true;
}


//Generic templated RequestHandler
template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
void tao::RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::Loop(ThreadManager& thread_manager, tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>& port, const std::string orb_endpoint, const std::string server_name){
    auto& helper = tao::TaoHelper::get_tao_helper();

    bool success = true;
    
    auto orb = helper.get_orb(orb_endpoint);

    if(!orb){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot get TAO Orb for endpoint: '" + orb_endpoint + "' ");
        success = false;
    }
    
    if(success){
        auto poa = helper.get_poa(orb, server_name);
        auto tao_server = new tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>(port);

        if(helper.register_servant(orb, poa, tao_server, server_name)){
            thread_manager.Thread_Configured();
        
            if(thread_manager.Thread_WaitForActivate()){
                port.LogActivation();
                thread_manager.Thread_Activated();
                thread_manager.Thread_WaitForTerminate();
            }
            helper.deregister_servant(orb, poa, tao_server, server_name);
            port.LogPassivation();
        }
    }

    thread_manager.Thread_Terminated();
};

#endif //TAO_INEVENTPORT_H
