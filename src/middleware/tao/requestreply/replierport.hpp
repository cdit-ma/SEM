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
    class TaoServerImpl;

    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    class ReplierPort : public ::ReplierPort<BaseReplyType, BaseRequestType>{
        //The Request Handle needs to be able to modify and change state of the Port
        friend class RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>;
        public:
            ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper);
            ~ReplierPort(){this->Terminate();};

            using middleware_reply_type = TaoReplyType;
            using middleware_request_type = TaoRequestType;
        protected:
            void HandleActivate();
            void HandleConfigure();
            void HandlePassivate();
            void HandleTerminate();
        private:
            std::unique_ptr<tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>> GetServer();
            void InterruptLoop();

            std::mutex mutex_;
            std::unique_ptr<ThreadManager> thread_manager_;
            
            std::shared_ptr<Attribute> server_name_;
            std::shared_ptr<Attribute> orb_endpoint_;
    };

    //Generic templated RequesterHandler
    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    struct RequestHandler{
        static void Loop(ThreadManager& thread_manager, std::unique_ptr<tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt> > server, const std::string orb_endpoint, const std::string server_address);
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

    template <class BaseReplyType, class TaoReplyType, class TaoServerInt>
    class TaoServerImpl<BaseReplyType, TaoReplyType, void, void, TaoServerInt> : public TaoServerInt{
        public:
            TaoServerImpl(tao::ReplierPort<BaseReplyType, TaoReplyType, void, void, TaoServerInt>& port):
                eventport(port){
            };
            TaoReplyType* TAO_SERVER_FUNC_NAME(){
                auto base_result = eventport.ProcessRequest();
                return Base::Translator<BaseReplyType, TaoReplyType>::BaseToMiddleware(base_result);
            };
        private:
            tao::ReplierPort<BaseReplyType, TaoReplyType, void, void, TaoServerInt>& eventport;
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
void tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::HandleConfigure(){
    auto server = GetServer();
    
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_)
        throw std::runtime_error("tao Replier Port: '" + this->get_name() + "': Has an errant ThreadManager!");
    
    thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
    auto future = std::async(std::launch::async, tao::RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::Loop, std::ref(*thread_manager_), std::move(server), orb_endpoint_->String(), server_name_->String());
    thread_manager_->SetFuture(std::move(future));
    thread_manager_->Configure();
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleConfigure();
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
void tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::HandleActivate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!thread_manager_)
        throw std::runtime_error("tao ReplierPort Port: '" + this->get_name() + "': Has no ThreadManager!");

    thread_manager_->Activate();
    //Wait for our recv thjread to be up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleActivate();
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
void tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::HandlePassivate(){
    InterruptLoop();
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate();
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
void tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(mutex_);
    thread_manager_.reset();
    ::ReplierPort<BaseReplyType, BaseRequestType>::HandleTerminate();
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
void tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::InterruptLoop(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_){
        thread_manager_->SetTerminate();
    }
}



template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
std::unique_ptr<tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>> tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::GetServer(){
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto& helper = tao::TaoHelper::get_tao_helper();
    auto orb = helper.get_orb(orb_endpoint_->String());

    if(orb){
        auto poa = helper.get_poa(orb, server_name_->String());
        auto server = std::unique_ptr<tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>>(
            new tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>(*this)
            );
        if(helper.register_servant(orb, poa, server.get(), server_name_->String())){
            return std::move(server);
        }
    }
    throw std::runtime_error("Cannot build TAO Impl");
}


//Generic templated RequestHandler
template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
void tao::RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::Loop(ThreadManager& thread_manager, std::unique_ptr<tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>> server, const std::string orb_endpoint, const std::string server_name){
    auto& helper = tao::TaoHelper::get_tao_helper();
    auto orb = helper.get_orb(orb_endpoint);
    auto poa = helper.get_poa(orb, server_name);
    
    thread_manager.Thread_Configured();
    if(thread_manager.Thread_WaitForActivate()){
        thread_manager.Thread_Activated();
        thread_manager.Thread_WaitForTerminate();
    }
    helper.deregister_servant(orb, poa, server.get(), server_name);
    
    server.reset();
    thread_manager.Thread_Terminated();
    std::cerr << "THREAD DED" << std::endl;
};

#endif //TAO_INEVENTPORT_H
