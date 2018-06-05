#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <memory>
#include <iostream>

#include <typeinfo>
#include <typeindex>

#include "ports/port.h"
class Worker;

#include "behaviourcontainer.h"
#include "basemessage.h"

class Component : public BehaviourContainer{
    public:
        Component(const std::string& component_name = "");
        virtual ~Component();

        //Port
        std::weak_ptr<Port> AddPort(std::unique_ptr<Port> port);
        std::weak_ptr<Port> GetPort(const std::string& port_name);
        template<class T>
        std::shared_ptr<T> GetTypedPort(const std::string& port_name);
        std::shared_ptr<Port> RemovePort(const std::string& port_name);
        
        template<class RequestType, class ReplyType>
        void AddCallback(const std::string& port_name, std::function<ReplyType (RequestType&)> function);

        template<class RequestType>
        void AddCallback(const std::string& port_name, std::function<void (RequestType&)> function);

        template<class ReplyType>
        void AddCallback(const std::string& port_name, std::function<ReplyType ()> function);

        template<class RequestType, class ReplyType>
        std::function<ReplyType (RequestType&)> GetCallback(const std::string& port_name);

        template<class RequestType>
        std::function<void (RequestType&)> GetCallback(const std::string& port_name);

        template<class ReplyType>
        std::function<ReplyType ()> GetCallbackParameterless(const std::string& port_name);

        
        bool AddPeriodicCallback(const std::string& port_name, std::function<void()> function);


        std::function<std::shared_ptr<::BaseMessage> (::BaseMessage*)> GetBaseCallback(const std::string& port_name);
        std::function<void (::BaseMessage*)> GetVoidCallback(const std::string& port_name);

        bool GotCallback(const std::string& port_name, const std::type_info& request_type, const std::type_info& reply_type);


        bool RemoveCallback(const std::string& port_name);
    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();
    private:
        //void AddVoidCallback_   (const std::string& port_name, const std::type_info& request_type,                                   std::function<void (::BaseMessage*)> function);
        void AddCallback_       (const std::string& port_name, const std::type_info& request_type, const std::type_info& reply_type, std::function<std::shared_ptr<::BaseMessage> (::BaseMessage*)> function);


        
        std::mutex state_mutex_;
        std::mutex port_mutex_;

        std::unordered_map<std::string, std::shared_ptr<Port> > ports_;
        std::unordered_map<std::string, std::function<std::shared_ptr<::BaseMessage> (::BaseMessage*)> > callback_functions_;

        std::unordered_map<std::string, std::function<void (::BaseMessage*)> > void_callback_functions_;
        
        std::unordered_map<std::string, std::pair<std::reference_wrapper<const std::type_info>, std::reference_wrapper<const std::type_info> > > callback_type_hash_;
};

template<class T>
std::shared_ptr<T> Component::GetTypedPort(const std::string& port_name){
    static_assert(std::is_base_of<Port, T>::value, "T must inherit from Port");
    auto p = GetPort(port_name).lock();
    return std::dynamic_pointer_cast<T>(p);
};

template<class RequestType, class ReplyType>
void Component::AddCallback(const std::string& port_name, std::function<ReplyType (RequestType&)> function){
    static_assert(std::is_base_of<::BaseMessage, RequestType>::value, "RequestType must inherit from ::BaseMessage");
    static_assert(std::is_base_of<::BaseMessage, ReplyType>::value, "ReplyType must inherit from ::BaseMessage");

    const auto& request_type = typeid(RequestType);
    const auto& reply_type = typeid(ReplyType);
    
    return AddCallback_(port_name, request_type, reply_type,
        [function] (::BaseMessage* base_request) -> std::shared_ptr<::BaseMessage>{
            //Convert ::BaseMessage* to ReplyType&
            auto& request = *(RequestType*) base_request;
            auto result = function(request);
            //Move result into a shared_ptr, which downcasts to std::shared_ptr<::BaseMessage>
            return std::make_shared<ReplyType>(std::move(result));
        });
};


template<class RequestType>
void Component::AddCallback(const std::string& port_name, std::function<void (RequestType&)> function){
    static_assert(std::is_base_of<::BaseMessage, RequestType>::value, "RequestType must inherit from ::BaseMessage");

    const auto& request_type = typeid(RequestType);
    const auto& reply_type = typeid(void);
    
    return AddCallback_(port_name, request_type, reply_type,
        [function] (::BaseMessage* base_request) -> std::shared_ptr<::BaseMessage>{
            //Convert ::BaseMessage* to ReplyType&
            auto& request = *(RequestType*) base_request;
            function(request);
            return nullptr;
        });
};

template<class ReplyType>
void Component::AddCallback(const std::string& port_name, std::function<ReplyType ()> function){
    static_assert(std::is_base_of<::BaseMessage, ReplyType>::value, "ReplyType must inherit from ::BaseMessage");

    const auto& request_type = typeid(void);
    const auto& reply_type = typeid(ReplyType);
    
    return AddCallback_(port_name, request_type, reply_type,
        [function] (::BaseMessage*) -> std::shared_ptr<::BaseMessage>{
            auto result = function();
            //Move result into a shared_ptr, which downcasts to std::shared_ptr<::BaseMessage>
            return std::make_shared<ReplyType>(std::move(result));
        });
};


template<class RequestType, class ReplyType>
std::function<ReplyType (RequestType&)> Component::GetCallback(const std::string& port_name){
    static_assert(std::is_base_of<::BaseMessage, RequestType>::value, "RequestType must inherit from ::BaseMessage");
    static_assert(std::is_base_of<::BaseMessage, ReplyType>::value, "ReplyType must inherit from ::BaseMessage");

    const auto& request_type = typeid(RequestType);
    const auto& reply_type = typeid(ReplyType);

    if(GotCallback(port_name, request_type, reply_type)){
        auto function = GetBaseCallback(port_name);
        return [function] (RequestType& message) -> ReplyType{
            //Run the function
            auto base_result = function(&message);
            if(base_result){
                //Get a ReturnType* of the result
                auto reply_type_ptr = (ReplyType*) base_result.get();
                return std::move(*reply_type_ptr);
            }else{
                return ReplyType();
            }
        };
    }
    return nullptr;
};

template<class RequestType>
std::function<void (RequestType&)> Component::GetCallback(const std::string& port_name){
    static_assert(std::is_base_of<::BaseMessage, RequestType>::value, "RequestType must inherit from ::BaseMessage");

    const auto& request_type = typeid(RequestType);
    const auto& reply_type = typeid(void);

    if(GotCallback(port_name, request_type, reply_type)){
        auto function = GetBaseCallback(port_name);
        return [function] (RequestType& message) -> void{
            //Run the function
            function(&message);
        };
    }
    return nullptr;
};


template<class ReplyType>
std::function<ReplyType ()> Component::GetCallbackParameterless(const std::string& port_name){
    static_assert(std::is_base_of<::BaseMessage, ReplyType>::value, "ReplyType must inherit from ::BaseMessage");

    const auto& request_type = typeid(void);
    const auto& reply_type = typeid(ReplyType);

    if(GotCallback(port_name, request_type, reply_type)){
        auto function = GetBaseCallback(port_name);
        return [function] () -> ReplyType{
            //Run the function
            auto base_result = function(nullptr);
            if(base_result){
                //Get a ReturnType* of the result
                auto reply_type_ptr = (ReplyType*) base_result.get();
                return std::move(*reply_type_ptr);
            }else{
                return ReplyType();
            }
        };
    }
    return nullptr;
};

#endif //COMPONENT_H