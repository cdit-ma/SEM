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
struct GenericCallbackWrapper{
    protected:
        GenericCallbackWrapper(){};
};

template <class ReplyType, class RequestType>
struct CallbackWrapper : GenericCallbackWrapper{
    CallbackWrapper(std::function<ReplyType (RequestType&)> fn)
    : callback_fn(fn){};

    using reply_type = ReplyType;
    using request_type = RequestType;

    std::function<ReplyType (RequestType&)> callback_fn;
};

template <class RequestType>
struct CallbackWrapper<void, RequestType> : GenericCallbackWrapper{
    CallbackWrapper(std::function<void (RequestType&)> fn)
    : callback_fn(fn){};

    using reply_type = void;
    using request_type = RequestType;

    std::function<void (RequestType&)> callback_fn;
};

template <class ReplyType>
struct CallbackWrapper<ReplyType, void> : GenericCallbackWrapper{
    CallbackWrapper(std::function<ReplyType ()> fn)
    : callback_fn(fn){};

    using reply_type = ReplyType;
    using request_type = void;

    std::function<ReplyType ()> callback_fn;
};


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
        

        template<class ReplyType, class RequestType>
        void AddCallback(const std::string& port_name, CallbackWrapper<ReplyType, RequestType>* wrapper);
        
        template<class ReplyType, class RequestType>
        CallbackWrapper<ReplyType, RequestType>* GetCallback(const std::string& port_name);


        bool GotCallback(const std::string& port_name, const std::type_info& request_type, const std::type_info& reply_type);
        bool RemoveCallback(const std::string& port_name);
    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();
    private:
        void AddCallback_(const std::string& port_name, const std::type_info& request_type, const std::type_info& reply_type, GenericCallbackWrapper* wrapper);


        std::mutex state_mutex_;
        std::mutex port_mutex_;

        std::unordered_map<std::string, std::shared_ptr<Port> > ports_;

        std::unordered_map<std::string, GenericCallbackWrapper*> callback_functions_;
        std::unordered_map<std::string, std::pair<std::reference_wrapper<const std::type_info>, std::reference_wrapper<const std::type_info> > > callback_type_hash_;
};

template<class T>
std::shared_ptr<T> Component::GetTypedPort(const std::string& port_name){
    static_assert(std::is_base_of<Port, T>::value, "T must inherit from Port");
    auto p = GetPort(port_name).lock();
    return std::dynamic_pointer_cast<T>(p);
};

template<class ReplyType, class RequestType>
void Component::AddCallback(const std::string& port_name, CallbackWrapper<ReplyType, RequestType>* wrapper){
    //static_assert(std::is_base_of<::BaseMessage, RequestType>::value, "RequestType must inherit from ::BaseMessage");
    //static_assert(std::is_base_of<::BaseMessage, ReplyType>::value, "ReplyType must inherit from ::BaseMessage");

    const auto& request_type = typeid(RequestType);
    const auto& reply_type = typeid(ReplyType);
    
    return AddCallback_(port_name, request_type, reply_type, wrapper);
};

template<class ReplyType, class RequestType>
CallbackWrapper<ReplyType, RequestType>* Component::GetCallback(const std::string& port_name){
    const auto& request_type = typeid(RequestType);
    const auto& reply_type = typeid(ReplyType);

    if(GotCallback(port_name, request_type, reply_type)){
        return (CallbackWrapper<ReplyType, RequestType>*)callback_functions_.at(port_name);
    }
    return nullptr;
};


#endif //COMPONENT_H