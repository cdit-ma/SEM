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
    // The default case for pointer TAO types
    template<class TaoType, bool big>
    struct TaoTraitHelper {
        typedef TaoType* return_type;
    };

    template<class TaoType>
    struct TaoTraitHelper<TaoType, true> {
        typedef TaoType return_type;
    };

    template<class TaoType>
    struct TaoTrait : TaoTraitHelper<TaoType, sizeof(TaoType) <= 4> {};

    template<class TaoType>
    struct DelayedTaoCast {
        DelayedTaoCast(TaoType *ptr) : m_ptr(ptr) {};
        operator TaoType*() const { return m_ptr; }
        operator TaoType() {
            TaoType result(*m_ptr);
            delete m_ptr;
            return result;
        }
        private:
            TaoType* m_ptr;
    };

    //helper function for instantiating DelayedTaoCast
    template<class TaoType>
    DelayedTaoCast<TaoType> delayedTaoCast(TaoType *ptr) {
        return DelayedTaoCast<TaoType>(ptr);
    };

    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    struct RequestHandler;

    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    class TaoServerImpl;

    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    class ReplierPort : public ::ReplierPort<BaseReplyType, BaseRequestType>{
        //The Request Handle needs to be able to modify and change state of the Port
        friend class RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>;
        friend class TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>;
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
            
            std::shared_ptr<Attribute> orb_endpoint_;
            std::shared_ptr<Attribute> naming_service_endpoint_;
            std::shared_ptr<Attribute> server_name_;
            std::shared_ptr<Attribute> server_kind_;
                        
            std::string current_naming_service_name_;
    };

    //Generic templated RequesterHandler
    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    struct RequestHandler{
        static void Loop(ThreadManager& thread_manager, std::unique_ptr<tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt> > server, const std::string orb_endpoint, const std::string naming_service_endpoint, const std::vector<std::string> server_name, const std::string server_kind);
    };

    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
    class TaoServerImpl : public TaoServerInt{
        public:
            TaoServerImpl(tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>& port):
                eventport(port){
            };
            typename TaoTrait<TaoReplyType>::return_type TAO_SERVER_FUNC_NAME(const TaoRequestType& message){
                try{
                    auto base_message = Base::Translator<BaseRequestType, TaoRequestType>::MiddlewareToBase(message);
                    auto base_result = eventport.ProcessRequest(*base_message);
                    auto tao_result_ptr = Base::Translator<BaseReplyType, TaoReplyType>::BaseToMiddleware(base_result);
                    return delayedTaoCast(tao_result_ptr);
                }catch(const std::exception& ex){
                    std::string error_str = "Translating Reply/Request Failed: ";
                    eventport.ProcessGeneralException(error_str + ex.what());
                    throw std::runtime_error(error_str + ex.what());
                }
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
                try{
                    auto base_message = Base::Translator<BaseRequestType, TaoRequestType>::MiddlewareToBase(message);
                    eventport.ProcessRequest(*base_message);
                    delete base_message;
                }catch(const std::exception& ex){
                    std::string error_str = "Translating Request Failed: ";
                    eventport.ProcessGeneralException(error_str + ex.what());
                    throw std::runtime_error(error_str + ex.what());
                }
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
            typename TaoTrait<TaoReplyType>::return_type TAO_SERVER_FUNC_NAME(){
                try{
                    auto base_result = eventport.ProcessRequest();
                    auto tao_result_ptr = Base::Translator<BaseReplyType, TaoReplyType>::BaseToMiddleware(base_result);
                    return delayedTaoCast(tao_result_ptr);
                }catch(const std::exception& ex){
                    std::string error_str = "Translating Reply Failed: ";
                    eventport.ProcessGeneralException(error_str + ex.what());
                    throw std::runtime_error(error_str + ex.what());
                }
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
    server_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "server_name").lock();
    naming_service_endpoint_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "naming_service_endpoint").lock();
    server_kind_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_kind").lock();

    current_naming_service_name_ = this->get_id() + "_" + this->get_name() + "_NamingService";
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
void tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::HandleConfigure(){
    auto server = GetServer();
    
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_)
        throw std::runtime_error("tao Replier Port: '" + this->get_name() + "': Has an errant ThreadManager!");
    
    thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
    auto future = std::async(std::launch::async, tao::RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::Loop, std::ref(*thread_manager_), std::move(server), orb_endpoint_->String(), current_naming_service_name_, server_name_->StringList(), server_kind_->String());
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
        auto poa_name = helper.GetPOAName(server_name_->StringList());
        auto poa = helper.get_poa(orb, poa_name);
        auto server = std::unique_ptr<tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>>(
            new tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>(*this)
            );
        
        //Register the nameserver
        helper.register_initial_reference(orb, current_naming_service_name_, naming_service_endpoint_->String());
        helper.register_servant_via_namingservice(orb, current_naming_service_name_, poa, server.get(), server_name_->StringList(), server_kind_->String());

        return std::move(server);
    }
    throw std::runtime_error("Cannot build TAO Impl");
}


//Generic templated RequestHandler
template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerInt>
void tao::RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>::Loop(ThreadManager& thread_manager, std::unique_ptr<tao::TaoServerImpl<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerInt>> server, const std::string orb_endpoint, const std::string current_naming_service_name, const std::vector<std::string> server_name, const std::string server_kind){
    auto& helper = tao::TaoHelper::get_tao_helper();
    auto orb = helper.get_orb(orb_endpoint);
    auto poa_name = helper.GetPOAName(server_name);
    auto poa = helper.get_poa(orb, poa_name);
    
    thread_manager.Thread_Configured();
    if(thread_manager.Thread_WaitForActivate()){
        thread_manager.Thread_Activated();
        thread_manager.Thread_WaitForTerminate();
    }
    helper.deregister_servant_via_namingservice(orb, current_naming_service_name, poa, server.get(), server_name, server_kind);
    
    server.reset();
    thread_manager.Thread_Terminated();
};

#endif //TAO_INEVENTPORT_H
