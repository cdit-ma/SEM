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

#include <boost/thread.hpp>

#include "ports/port.h"
class Worker;

#include "behaviourcontainer.h"
#include "basemessage.h"

// REVIEW (Mitch): Move callback wrapper into its own file.
// REVIEW (Mitch): Trivial constructor, use =0

// REVIEW (Mitch): Alternatively use decltype at usage sites to determine callback's type.
struct GenericCallbackWrapper{
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

template <>
struct CallbackWrapper<void, void> : GenericCallbackWrapper{
    CallbackWrapper(std::function<void ()> fn)
    : callback_fn(fn){};

    using reply_type = void;
    using request_type = void;

    std::function<void ()> callback_fn;
};

// REVIEW (Mitch): Enforce, at an organisation level, unit testing of all functions called only from
//  code generated code.
class Component : public BehaviourContainer{
    // REVIEW (Mitch): Friend class DeploymentContainer, why??
    friend class DeploymentContainer;
    public:
        Component(const std::string& component_name = "");
        virtual ~Component();

        //Port
        std::weak_ptr<Port> AddPort(std::unique_ptr<Port> port);
        std::weak_ptr<Port> GetPort(const std::string& port_name);

        template<class PortType>
        std::shared_ptr<PortType> GetTypedPort(const std::string& port_name);

        template<class PortType>
        std::shared_ptr<PortType> SafeGetTypedPort(const std::string& port_name);

        template<class ReplyType, class RequestType>
        void RegisterCallback(const std::string& port_name, std::function<ReplyType (RequestType&)> fn);

        template<class ReplyType, class RequestType>
        void RegisterCallback(const std::string& port_name, std::function<ReplyType (void)> fn);

        // REVIEW (Mitch): Why does this function exist in Component?
        //  Usage pattern for this function seems to be detailed in test/core/periodicevent/tests.cpp
        // Consider redesigning how periodic callbacks are registered.
        // Why does the component need to know about it's periodic port's callback?
        void RegisterPeriodicCallback(const std::string& port_name, std::function<void ()> fn){
            RegisterCallback<void, BaseMessage>(port_name, [=](BaseMessage&){
                fn();
            });
        };
        
        template<class ReplyType, class RequestType>
        const CallbackWrapper<ReplyType, RequestType>& GetCallback(const std::string& port_name);

        bool GotCallback(const std::string& port_name, const std::type_info& request_type, const std::type_info& reply_type);
        bool RemoveCallback(const std::string& port_name);

        const std::vector<std::string>& GetLocation() const;
        const std::vector<int>& GetReplicationIndices() const;
        std::string GetLocalisedName() const;
        const std::string& GetExperimentName() const;
    protected:
        virtual void HandleActivate();
        virtual void HandleConfigure();
        virtual void HandlePassivate();
        virtual void HandleTerminate();

        void SetLocation(const std::vector<std::string>& location);
        void SetReplicationIndices(const std::vector<int>& indices);
        void SetExperimentName(const std::string& experiment_name);
    private:
        template<class ReplyType, class RequestType>
        void AddCallback(const std::string& port_name, std::unique_ptr< CallbackWrapper<ReplyType, RequestType> > wrapper);
        // REVIEW (Mitch): Replace type_info with type_index
        void AddCallback(const std::string& port_name, const std::type_info& request_type, const std::type_info& reply_type, std::unique_ptr<GenericCallbackWrapper> wrapper);

        boost::shared_mutex port_mutex_;
        std::unordered_map<std::string, std::shared_ptr<Port> > ports_;
        std::unordered_map<std::string, std::unique_ptr<GenericCallbackWrapper> > callback_functions_;
        // REVIEW (Mitch): Is there any reason for the type_info pair to be ref_wrappered?
        //  Replace with std::type_index
        std::unordered_map<std::string, std::pair<std::reference_wrapper<const std::type_info>, std::reference_wrapper<const std::type_info> > > callback_type_hash_;

        std::vector<std::string> component_location_;
        std::vector<int> replication_indices_;
        std::string experiment_name_;
};

template<class PortType>
std::shared_ptr<PortType> Component::GetTypedPort(const std::string& port_name){
    static_assert(std::is_base_of<Port, PortType>::value, "PortType must inherit from Port");
    return std::dynamic_pointer_cast<PortType>(GetPort(port_name).lock());
};


template<class PortType>
std::shared_ptr<PortType> Component::SafeGetTypedPort(const std::string& port_name){
    // REVIEW (Mitch): read "process_event" as "can_process_event" (check to see if Activatable is
    //  both running and not currently transitioning)
    if(process_event()){
        return GetTypedPort<PortType>(port_name);
    }
    return nullptr;
};

template<class ReplyType, class RequestType>
void Component::AddCallback(const std::string& port_name, std::unique_ptr<CallbackWrapper<ReplyType, RequestType>> wrapper){
    const auto& request_type = typeid(RequestType);
    const auto& reply_type = typeid(ReplyType);
    return AddCallback(port_name, request_type, reply_type, std::move(wrapper));
};

template<class ReplyType, class RequestType>
void Component::RegisterCallback(const std::string& port_name, std::function<ReplyType (RequestType&)> fn){
    auto callback = std::unique_ptr< CallbackWrapper<ReplyType, RequestType> >(new CallbackWrapper<ReplyType, RequestType>(fn));
    AddCallback<ReplyType, RequestType>(port_name, std::move(callback));
};

template<class ReplyType, class RequestType>
void Component::RegisterCallback(const std::string& port_name, std::function<ReplyType (void)> fn){
    static_assert(std::is_void<RequestType>::value, "RequestType must be void");
    auto callback = std::unique_ptr< CallbackWrapper<ReplyType, RequestType> >(new CallbackWrapper<ReplyType, RequestType>(fn));
    AddCallback<ReplyType, RequestType>(port_name, std::move(callback));
};

template<class ReplyType, class RequestType>
const CallbackWrapper<ReplyType, RequestType>& Component::GetCallback(const std::string& port_name){
    const auto& request_type = typeid(RequestType);
    const auto& reply_type = typeid(ReplyType);

    if(GotCallback(port_name, request_type, reply_type)){
        return (const CallbackWrapper<ReplyType, RequestType>&) (* callback_functions_.at(port_name));
    }else{
        throw std::runtime_error("Component: '" + get_name() + "' does not have a matching registered Port Callback: '" + port_name + "'");
    }
};

#endif //COMPONENT_H