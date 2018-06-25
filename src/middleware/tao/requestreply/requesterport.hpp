#ifndef TAO_PORT_REQUESTER_HPP
#define TAO_PORT_REQUESTER_HPP

#ifndef TAO_SERVER_FUNC_NAME
//REQUIRES MACRO TAO_SERVER_FUNC_NAME TO BE DEFINED
static_assert(false, "Requires TAO_SERVER_FUNC_NAME To Be Defined")
#endif


#include <core/ports/requestreply/requesterport.hpp>
#include <middleware/tao/taohelper.h>

#include <string>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <set>


namespace tao{
    //Generic templated RequesterPort
    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
    class RequesterPort : public ::RequesterPort<BaseReplyType, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            };

            BaseReplyType ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);

            using middleware_reply_type = TaoReplyType;
            using middleware_request_type = TaoRequestType;
        protected:
            bool HandleConfigure();
            bool HandleTerminate();
        public:
            std::shared_ptr<Attribute> server_address_;
            std::shared_ptr<Attribute> server_name_;
            std::shared_ptr<Attribute> orb_endpoint_;
            
            int count = 0;
            std::string current_server_name_;
            
            std::mutex control_mutex_;

            CORBA::ORB_var orb_ = 0;
            std::mutex client_mutex_;
    };

    //Specialised templated RequesterPort for void returning
    template <class BaseRequestType, class TaoRequestType, class TaoClientImpl>
    class RequesterPort<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>: public ::RequesterPort<void, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            };
            
            void ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);
        protected:
            bool HandleConfigure();
            bool HandleTerminate();

            using middleware_reply_type = void;
            using middleware_request_type = TaoRequestType;
        public:
            std::shared_ptr<Attribute> server_address_;
            std::shared_ptr<Attribute> server_name_;
            std::shared_ptr<Attribute> orb_endpoint_;
            
            int count = 0;
            std::string current_server_name_;
            
            std::mutex control_mutex_;

            CORBA::ORB_var orb_ = 0;
            std::mutex client_mutex_;
    };

    //Specialised templated RequesterPort for void requesting
    template <class BaseReplyType, class TaoReplyType, class TaoClientImpl>
    class RequesterPort<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>: public ::RequesterPort<BaseReplyType, void>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            };

            BaseReplyType ProcessRequest(std::chrono::milliseconds timeout);
        protected:
            bool HandleConfigure();
            bool HandleTerminate();

            using middleware_reply_type = TaoReplyType;
            using middleware_request_type = void;
        public:
            std::shared_ptr<Attribute> server_address_;
            std::shared_ptr<Attribute> server_name_;
            std::shared_ptr<Attribute> orb_endpoint_;
            
            int count = 0;
            std::string current_server_name_;
            
            std::mutex control_mutex_;

            CORBA::ORB_var orb_ = 0;
            std::mutex client_mutex_;
    };

    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
    static bool SetupRequester(tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>& port, const std::string& orb_endpoint, const std::string& server_address, const std::string& server_name);
};


//Generic templated ReplierPort
template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, BaseRequestType>(component, port_name, "tao"){
    orb_endpoint_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "orb_endpoint").lock();
    server_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_name").lock();
    server_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
bool tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    const auto orb_endpoint = orb_endpoint_->String();
    const auto server_name = server_name_->String();
    const auto server_address = server_address_->String();

    bool valid = orb_endpoint.size() > 0 && server_name.size() > 0 && server_address.size() > 0;

    if(valid && ::RequesterPort<BaseReplyType, BaseRequestType>::HandleConfigure()){
        return tao::SetupRequester<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>(*this, orb_endpoint, server_address, server_name);
    }
    return false;
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
bool tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::RequesterPort<BaseReplyType, BaseRequestType>::HandleTerminate()){
        std::lock_guard<std::mutex> client_lock(client_mutex_);
        return true; 
    }
    return false; 
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
bool tao::SetupRequester(
    tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>& port, 
    const std::string& orb_endpoint,
    const std::string& server_address,
    const std::string& server_name)
{
    std::lock_guard<std::mutex> client_lock(port.client_mutex_);
    auto& helper = tao::TaoHelper::get_tao_helper();
    port.orb_ = helper.get_orb(orb_endpoint);
    auto count = port.count ++;
    port.current_server_name_ = port.get_id() + "_" + server_name + "_" + std::to_string(count);

    helper.register_initial_reference(port.orb_, port.current_server_name_, server_address);
    return true;
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
BaseReplyType tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::ProcessRequest(const BaseRequestType& message, std::chrono::milliseconds timeout){
    std::lock_guard<std::mutex> lock(client_mutex_); 
    
    const auto orb_endpoint = orb_endpoint_->String();
    const auto& cached_server_name = current_server_name_;
    try{
        auto& helper = tao::TaoHelper::get_tao_helper();
        auto ptr = helper.resolve_initial_references(orb_, cached_server_name);
        auto client = TaoClientImpl::_unchecked_narrow(ptr);

        /*
        Timed Request
        TimeBase::TimeT time_value = 10000;
        CORBA::Any object_timeout;
        any_object <<= time_value;

        CORBA::PolicyList policy_list (1);
        policy_list.length (1);
        policy_list[0] = orb_->create_policy (Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE, object_timeout);
        
        auto timed_client_ptr = client->_set_policy_overrides(policy_list, CORBA::SET_OVERRIDE);
        auto timed_client = TaoClientImpl::_narrow(timed_client_ptr);
        policy_list[0]->destroy ();*/

        
        //Another interesting TAO feature is the support for _unchecked_narrow()
        //This is part of the CORBA Messaging specification and essentially performs the same work as _narrow(),
        //but it does not check the types remotely. If you have compile time knowledge that ensures the correctness of the narrow operation,
        //it is more efficient to use the unchecked version.
        if(client){
            auto request_ptr = Base::Translator<BaseRequestType, TaoRequestType>::BaseToMiddleware(message);

            TaoReplyType* reply_ptr = client->TAO_SERVER_FUNC_NAME(*request_ptr);
            auto base_reply_ptr = Base::Translator<BaseReplyType, TaoReplyType>::MiddlewareToBase(*reply_ptr);

            //Copy the message into a heap allocated object
            BaseReplyType base_reply(*base_reply_ptr);
            
            //Clean up the memory from the base_reply_ptr
            delete base_reply_ptr;

            //delete reply_ptr;
            CORBA::release(client);
            return base_reply;
        }
    }catch (const CORBA::TIMEOUT &timeout) {
        throw std::runtime_error("Timeout");
    }catch(const CORBA::Exception& e){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot Get Client: '" + cached_server_name + "' " + e._name());
        throw std::runtime_error("Corba Exception");
    }
}



//Specialised templated RequesterPort for void returning
template <class BaseRequestType, class TaoRequestType, class TaoClientImpl>
tao::RequesterPort<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<void, BaseRequestType>(component, port_name, "tao"){
    orb_endpoint_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "orb_endpoint").lock();
    server_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_name").lock();
    server_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseRequestType, class TaoRequestType, class TaoClientImpl>
bool tao::RequesterPort<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    const auto orb_endpoint = orb_endpoint_->String();
    const auto server_name = server_name_->String();
    const auto server_address = server_address_->String();

    bool valid = orb_endpoint.size() > 0 && server_name.size() > 0 && server_address.size() > 0;

    if(valid && ::RequesterPort<void, BaseRequestType>::HandleConfigure()){
        return tao::SetupRequester<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>(*this, orb_endpoint, server_address, server_name);
    }
    return false;
};


template <class BaseRequestType, class TaoRequestType, class TaoClientImpl>
bool tao::RequesterPort<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::RequesterPort<void, BaseRequestType>::HandleTerminate()){
        std::lock_guard<std::mutex> client_lock(client_mutex_);
        return true; 
    }
    return false; 
};

template <class BaseRequestType, class TaoRequestType, class TaoClientImpl>
void tao::RequesterPort<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>::ProcessRequest(const BaseRequestType& message, std::chrono::milliseconds timeout){
    std::lock_guard<std::mutex> lock(client_mutex_); 
    
    const auto orb_endpoint = orb_endpoint_->String();
    const auto& cached_server_name = current_server_name_;
    try{
        auto& helper = tao::TaoHelper::get_tao_helper();
        auto ptr = helper.resolve_initial_references(orb_, cached_server_name);
        auto client = TaoClientImpl::_unchecked_narrow(ptr);

        //Another interesting TAO feature is the support for _unchecked_narrow()
        //This is part of the CORBA Messaging specification and essentially performs the same work as _narrow(),
        //but it does not check the types remotely. If you have compile time knowledge that ensures the correctness of the narrow operation,
        //it is more efficient to use the unchecked version.
        if(client){
            auto request_ptr = Base::Translator<BaseRequestType, TaoRequestType>::BaseToMiddleware(message);
            client->TAO_SERVER_FUNC_NAME(*request_ptr);
            CORBA::release(client);
            return;
        }
    }catch (const CORBA::TIMEOUT &timeout) {
        throw std::runtime_error("Timeout");
    }catch(const CORBA::Exception& e){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot Get Client: '" + cached_server_name + "' " + e._name());
        throw std::runtime_error("Corba Exception");
    }
}



//Specialised templated RequesterPort for void requesting
template <class BaseReplyType, class TaoReplyType, class TaoClientImpl>
tao::RequesterPort<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, void>(component, port_name, "tao"){
    orb_endpoint_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "orb_endpoint").lock();
    server_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_name").lock();
    server_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseReplyType, class TaoReplyType, class TaoClientImpl>
bool tao::RequesterPort<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    const auto orb_endpoint = orb_endpoint_->String();
    const auto server_name = server_name_->String();
    const auto server_address = server_address_->String();

    bool valid = orb_endpoint.size() > 0 && server_name.size() > 0 && server_address.size() > 0;

    if(valid && ::RequesterPort<BaseReplyType, void>::HandleConfigure()){
        return tao::SetupRequester<BaseReplyType, BaseRequestType, void, void, TaoClientImpl>(*this, orb_endpoint, server_address, server_name);
    }
    return false;
};


template <class BaseReplyType, class TaoReplyType, class TaoClientImpl>
bool tao::RequesterPort<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::RequesterPort<BaseReplyType, void>::HandleTerminate()){
        std::lock_guard<std::mutex> client_lock(client_mutex_);
        return true; 
    }
    return false; 
};

template <class BaseReplyType, class TaoReplyType, class TaoClientImpl>
BaseReplyType tao::RequesterPort<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>::ProcessRequest(const BaseRequestType& message, std::chrono::milliseconds timeout){
    std::lock_guard<std::mutex> lock(client_mutex_); 
    
    const auto orb_endpoint = orb_endpoint_->String();
    const auto& cached_server_name = current_server_name_;
    try{
        auto& helper = tao::TaoHelper::get_tao_helper();
        auto ptr = helper.resolve_initial_references(orb_, cached_server_name);
        auto client = TaoClientImpl::_unchecked_narrow(ptr);

        //Another interesting TAO feature is the support for _unchecked_narrow()
        //This is part of the CORBA Messaging specification and essentially performs the same work as _narrow(),
        //but it does not check the types remotely. If you have compile time knowledge that ensures the correctness of the narrow operation,
        //it is more efficient to use the unchecked version.
        if(client){
            TaoReplyType* reply_ptr = client->TAO_SERVER_FUNC_NAME();
            auto base_reply_ptr = Base::Translator<BaseReplyType, TaoReplyType>::MiddlewareToBase(*reply_ptr);

            //Copy the message into a heap allocated object
            BaseReplyType base_reply(*base_reply_ptr);
            
            //Clean up the memory from the base_reply_ptr
            delete base_reply_ptr;

            //delete reply_ptr;
            CORBA::release(client);
            return base_reply;
        }
    }catch (const CORBA::TIMEOUT &timeout) {
        throw std::runtime_error("Timeout");
    }catch(const CORBA::Exception& e){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot Get Client: '" + cached_server_name + "' " + e._name());
        throw std::runtime_error("Corba Exception");
    }
}



#endif //TAO_PORT_REQUESTER_HPP
