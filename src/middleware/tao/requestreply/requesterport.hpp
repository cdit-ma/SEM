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
            ~RequesterPort(){this->Terminate();};

            BaseReplyType ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);

            using middleware_reply_type = TaoReplyType;
            using middleware_request_type = TaoRequestType;
        protected:
            void HandleConfigure();
            void HandleTerminate();
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
            ~RequesterPort(){this->Terminate();};
            
            void ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);
        protected:
            void HandleConfigure();
            void HandleTerminate();

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
            ~RequesterPort(){this->Terminate();};

            BaseReplyType ProcessRequest(std::chrono::milliseconds timeout);
        protected:
            void HandleConfigure();
            void HandleTerminate();

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
    static void SetupRequester(tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>& port);
    
    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
    static void TeardownRequester(tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>& port);
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
void tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleConfigure(){
    tao::SetupRequester<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>(*this);
    ::RequesterPort<BaseReplyType, BaseRequestType>::HandleConfigure();
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
void tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleTerminate(){
    tao::TeardownRequester<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>(*this);
    ::RequesterPort<BaseReplyType, BaseRequestType>::HandleTerminate();
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
void tao::SetupRequester(tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>& port)
{
    //Gain the mutex for the client
    std::lock_guard<std::mutex> client_lock(port.client_mutex_);
    const auto& orb_endpoint = port.orb_endpoint_->String();
    const auto& server_address = port.server_address_->String();
    const auto& server_name = port.server_name_->String();

    auto& helper = tao::TaoHelper::get_tao_helper();
    port.orb_ = helper.get_orb(orb_endpoint);
    auto count = port.count ++;
    port.current_server_name_ = port.get_id() + "_" + server_name + "_" + std::to_string(count);
    helper.register_initial_reference(port.orb_, port.current_server_name_, server_address);
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
void tao::TeardownRequester(tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>& port)
{
    //Gain the mutex for the client
    std::lock_guard<std::mutex> client_lock(port.client_mutex_);
    if(port.count > 0 && port.orb_){
        auto& helper = tao::TaoHelper::get_tao_helper();
        helper.deregister_initial_reference(port.orb_, port.current_server_name_);
    }
};



template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
BaseReplyType tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::ProcessRequest(const BaseRequestType& message, std::chrono::milliseconds timeout){
    std::lock_guard<std::mutex> lock(client_mutex_); 
    
    const auto orb_endpoint = orb_endpoint_->String();
    const auto& cached_server_name = current_server_name_;
    try{
        auto& helper = tao::TaoHelper::get_tao_helper();
        //auto ptr = helper.resolve_initial_references(orb_, cached_server_name);
        auto ptr = helper.resolve_ns_references(orb_, server_name_->String());
        std::cerr << "FOUND: " << ptr << std::endl;

        auto client = TaoClientImpl::_unchecked_narrow(ptr);
        

        //10e4 difference between 1 milliseconds and 100 nanoseconds
        //Timed Request 1 = 100 nanoseconds
        TimeBase::TimeT time_value = timeout.count() * 1e4;
        CORBA::Any object_timeout;
        object_timeout <<= time_value;
        
        CORBA::PolicyList policy_list (1);
        policy_list.length (1);
        policy_list[0] = orb_->create_policy (Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE, object_timeout);
        
        auto client_ptr = client->_set_policy_overrides(policy_list, CORBA::SET_OVERRIDE);
        auto timeout_client = TaoClientImpl::_narrow(client_ptr);
        
        policy_list[0]->destroy();
        policy_list[0] = CORBA::Policy::_nil();

        //Another interesting TAO feature is the support for _unchecked_narrow()
        //This is part of the CORBA Messaging specification and essentially performs the same work as _narrow(),
        //but it does not check the types remotely. If you have compile time knowledge that ensures the correctness of the narrow operation,
        //it is more efficient to use the unchecked version.
        if(timeout_client){
            auto request_ptr = Base::Translator<BaseRequestType, TaoRequestType>::BaseToMiddleware(message);
            auto reply_ptr = timeout_client->TAO_SERVER_FUNC_NAME(*request_ptr);

            auto base_reply_ptr = Base::Translator<BaseReplyType, TaoReplyType>::MiddlewareToBase(*reply_ptr);

            //Copy the message into a heap allocated object
            BaseReplyType base_reply(*base_reply_ptr);
            
            //Clean up the memory from the base_reply_ptr
            delete request_ptr;
            delete base_reply_ptr;
            delete reply_ptr;
            CORBA::release(timeout_client);
            CORBA::release(client);
            return base_reply;
        }else{
            std::cerr << "HELLO" << std::endl;
        }
    }catch (const CORBA::TIMEOUT &timeout) {
        std::cerr << "HELLO" << std::endl;
        throw std::runtime_error("Timeout");
    }catch(const CORBA::Exception& e){
        std::cerr << "HELLO" << std::endl;
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
void tao::RequesterPort<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleConfigure(){
    tao::SetupRequester<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>(*this);
    ::RequesterPort<void, BaseRequestType>::HandleConfigure();
};


template <class BaseRequestType, class TaoRequestType, class TaoClientImpl>
void tao::RequesterPort<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleTerminate(){
    tao::TeardownRequester<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>(*this);
    ::RequesterPort<void, BaseRequestType>::HandleTerminate();
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

        //10e4 difference between 1 milliseconds and 100 nanoseconds
        //Timed Request 1 = 100 nanoseconds
        TimeBase::TimeT time_value = timeout.count() * 1e4;
        CORBA::Any object_timeout;
        object_timeout <<= time_value;
        
        CORBA::PolicyList policy_list (1);
        policy_list.length (1);
        policy_list[0] = orb_->create_policy (Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE, object_timeout);
        
        auto client_ptr = client->_set_policy_overrides(policy_list, CORBA::SET_OVERRIDE);
        auto timeout_client = TaoClientImpl::_narrow(client_ptr);
        
        policy_list[0]->destroy();
        policy_list[0] = CORBA::Policy::_nil();

        //Another interesting TAO feature is the support for _unchecked_narrow()
        //This is part of the CORBA Messaging specification and essentially performs the same work as _narrow(),
        //but it does not check the types remotely. If you have compile time knowledge that ensures the correctness of the narrow operation,
        //it is more efficient to use the unchecked version.
        if(timeout_client){
            auto request_ptr = Base::Translator<BaseRequestType, TaoRequestType>::BaseToMiddleware(message);
            timeout_client->TAO_SERVER_FUNC_NAME(*request_ptr);
            
            //Clean up the memory from the base_reply_ptr
            CORBA::release(timeout_client);
            CORBA::release(client);
            delete request_ptr;
        }
    }catch (const CORBA::TIMEOUT &timeout) {
        std::cerr << "TIMEOUT" << std::endl;
        throw std::runtime_error("Timeout");
    }catch(const CORBA::Exception& e){
        std::cerr << "TIMEOUT" << std::endl;
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
void tao::RequesterPort<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>::HandleConfigure(){
    tao::SetupRequester<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>(*this);
    ::RequesterPort<BaseReplyType, void>::HandleConfigure();
};


template <class BaseReplyType, class TaoReplyType, class TaoClientImpl>
void tao::RequesterPort<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>::HandleTerminate(){
    tao::TeardownRequester<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>(*this);
    ::RequesterPort<BaseReplyType, void>::HandleTerminate();
};

template <class BaseReplyType, class TaoReplyType, class TaoClientImpl>
BaseReplyType tao::RequesterPort<BaseReplyType, TaoReplyType, void, void, TaoClientImpl>::ProcessRequest(std::chrono::milliseconds timeout){
    std::lock_guard<std::mutex> lock(client_mutex_); 
    
    const auto orb_endpoint = orb_endpoint_->String();
    const auto& cached_server_name = current_server_name_;
    try{
        auto& helper = tao::TaoHelper::get_tao_helper();
        auto ptr = helper.resolve_initial_references(orb_, cached_server_name);
        
        auto client = TaoClientImpl::_unchecked_narrow(ptr);

        //10e4 difference between 1 milliseconds and 100 nanoseconds
        //Timed Request 1 = 100 nanoseconds
        TimeBase::TimeT time_value = timeout.count() * 1e4;
        CORBA::Any object_timeout;
        object_timeout <<= time_value;
        
        CORBA::PolicyList policy_list (1);
        policy_list.length (1);
        policy_list[0] = orb_->create_policy (Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE, object_timeout);
        
        auto client_ptr = client->_set_policy_overrides(policy_list, CORBA::SET_OVERRIDE);
        auto timeout_client = TaoClientImpl::_narrow(client_ptr);
        
        policy_list[0]->destroy();
        policy_list[0] = CORBA::Policy::_nil();

        //Another interesting TAO feature is the support for _unchecked_narrow()
        //This is part of the CORBA Messaging specification and essentially performs the same work as _narrow(),
        //but it does not check the types remotely. If you have compile time knowledge that ensures the correctness of the narrow operation,
        //it is more efficient to use the unchecked version.
        if(timeout_client){
            auto reply_ptr = timeout_client->TAO_SERVER_FUNC_NAME();
            auto base_reply_ptr = Base::Translator<BaseReplyType, TaoReplyType>::MiddlewareToBase(*reply_ptr);

            //Copy the message into a heap allocated object
            BaseReplyType base_reply(*base_reply_ptr);
            
            //Clean up the memory from the base_reply_ptr
            delete base_reply_ptr;
            delete reply_ptr;
            CORBA::release(timeout_client);
            CORBA::release(client);
            return base_reply;
        }
    }catch (const CORBA::TIMEOUT &timeout) {
        throw std::runtime_error("Timeout");
    }catch(const CORBA::Exception& e){
        throw std::runtime_error("Corba Exception");
    }
}



#endif //TAO_PORT_REQUESTER_HPP
