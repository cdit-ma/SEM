#ifndef TAO_PORT_REPLIER_HPP
#define TAO_PORT_REPLIER_HPP

#include <core/threadmanager.h>
#include <core/ports/requestreply/replierport.hpp>
#include <middleware/tao/taohelper.h>

//Inspired by
//https://www.codeproject.com/Articles/24863/A-Simple-C-Client-Server-in-CORBA

namespace tao{
    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
    class ReplierPort : public ::ReplierPort<BaseReplyType, BaseRequestType>{
        public:
            ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > server_function);
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
            
            std::shared_ptr<Attribute> publisher_name_;
            std::shared_ptr<Attribute> orb_endpoint_;
    };

    //Generic templated RequesterHandler
    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
    struct RequestHandler{
        static void Loop(ThreadManager& thread_manager, tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>& port, const std::string orb_endpoint, const std::string publisher_name);
    };
};

//Generic templated ReplierPort
template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name,  std::function<BaseReplyType (BaseRequestType&) > server_function):
::ReplierPort<BaseReplyType, BaseRequestType>(component, port_name, server_function, "tao"){
    orb_endpoint_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "orb_endpoint").lock();
    publisher_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "publisher_name").lock();
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    const auto orb_endpoint = orb_endpoint_->String();
    const auto publisher_name = publisher_name_->String();

    bool valid = orb_endpoint.size() > 0 && publisher_name.size() > 0;
    if(valid && ::ReplierPort<BaseReplyType, BaseRequestType>::HandleConfigure()){
        if(!thread_manager_){
            thread_manager_ = new ThreadManager();
            auto thread = std::unique_ptr<std::thread>(new std::thread(tao::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop, std::ref(*thread_manager_), std::ref(*this), orb_endpoint, publisher_name));
            thread_manager_->SetThread(std::move(thread));
            return thread_manager_->Configure();
        }else{
            std::cerr << "STILL GOT THREAD" << std::endl;
        }
    }
    return false;
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>::HandleActivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandleActivate()){
        return thread_manager_->Activate();
    }
    return false;
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Call into the base Handle Passivate
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate()){
        return TerminateThread();
    }
    return false;
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Call into the base Handle Passivate
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate()){
        return TerminateThread();
    }
    return false;
};



template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>::HandleTerminate(){
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


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
bool tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>::TerminateThread(){
    if(thread_manager_){
        return thread_manager_->Terminate();
    }
    return true;
}


//Generic templated RequestHandler
template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoServerImpl>
void tao::RequestHandler<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>::Loop(ThreadManager& thread_manager, tao::ReplierPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoServerImpl>& port, const std::string orb_endpoint, const std::string publisher_address){
    auto& helper = tao::TaoHelper::get_tao_helper();

    bool success = true;
    
    auto orb = helper.get_orb(orb_endpoint);

    if(!orb){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot get TAO Orb for endpoint: '" + orb_endpoint + "' ");
        success = false;
    }

    auto poa = helper.get_poa(orb, publisher_name);
    auto tao_server = new TaoServerImpl(*this);
            
    //Activate WITH ID
    //Convert our string into an object_id
    helper.register_servant(orb, poa, tao_server, publisher_name);
    poa->the_POAManager()->activate();

    if(success){
        thread_manager.Thread_Configured();

        if(thread_manager.Thread_WaitForActivate()){
            port.LogActivation();
            thread_manager.Thread_Activated();

            thread_manager.Thread_WaitForTerminate();
        }

        PortableServer::POA_var poa = recv->_default_POA ();
        PortableServer::ObjectId_var id = poa->servant_to_id (recv);
        poa->deactivate_object (id.in ());
        poa->destroy (true, true);
        //Log that the port has been passivated
        port.LogPassivation();
    }

    thread_manager.Thread_Terminated();
};

#endif //TAO_INEVENTPORT_H
