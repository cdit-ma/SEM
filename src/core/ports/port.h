#ifndef BASE_PORT_H
#define BASE_PORT_H

#include "../activatable.h"
#include "../basemessage.h"

#include <atomic>
#include <cstdint>

//Forward declare
class Component;

// REVIEW (Mitch): Never used (in non generated code at least, have not reviewed generated code)
enum class ThreadState{WAITING, STARTED, ACTIVE, TERMINATED};

class CallbackException : public std::runtime_error{
    public:
        CallbackException(const std::string& what_arg) : std::runtime_error(std::string("Callback Exception: ") + what_arg){};
};

//Interface for a standard Port
// REVIEW (Mitch): Move to actually being an interface
//  Potentially rename to better reflect that this is closer to a `task` interface.
//  Implementors of interface should be able to specify how the task is triggered (periodic, callback, subscriber port etc...)
class Port : public Activatable
{
    public:
        enum class Kind{
            NONE = 0,
            PERIODIC = 1,
            PUBLISHER = 2,
            SUBSCRIBER = 3,
            REQUESTER = 4,
            REPLIER = 5
        };

        // REVIEW (Mitch): Consider config struct initialisation.
        // REVIEW (Mitch): This constructor becomes Port(ComponentID, port_name, Middleware) after all suggested refactors.
        Port(std::weak_ptr<Component> component, const std::string& port_name, const Port::Kind& port_kind, const std::string& port_middleware);
        
        Port::Kind get_kind() const;
        std::string get_middleware() const;
        std::weak_ptr<Component> get_component() const;

        // REVIEW (Mitch): Mark as const
        uint64_t GetEventsReceived();
        uint64_t GetEventsProcessed();
        uint64_t GetEventsIgnored();
    protected:
        void HandleConfigure();
        void HandlePassivate();
        void HandleTerminate();
        void HandleActivate();

        // REVIEW (Mitch): Why do EventRecEIved and EventProcessed take an argument? They only increment a counter
        void EventRecieved(const BaseMessage& message);
        void EventProcessed(const BaseMessage& message);
        void EventIgnored(const BaseMessage& message);
        void ProcessMessageException(const BaseMessage& message, const std::string& error_str);
        void ProcessGeneralException(const std::string& error_str);

        // REVIEW (Mitch): This is only ever called in PeriodicPort's constructor.
        //  Periodic port continues to be a special snowflake, consider rework.
        void SetKind(const Port::Kind& port_kind);
    private:
    // REVIEW (Mitch): Replace component ref with parent component ID.
        std::weak_ptr<Component> component_;
    // REVIEW (Mitch): Why is this a string? Investigate replacing all instances of middleware (stored as string)
    //  with an enum (specialisations of this enum required for req/rep vs pub/sub).
        std::string port_middleware_;
        // REVIEW (Mitch): Is a "invalid" type value needed here? Is this only done for initialisation reasons?
        Port::Kind port_kind_ = Port::Kind::NONE;

        std::atomic<uint64_t> received_count_{0};
        std::atomic<uint64_t> processed_count_{0};
        std::atomic<uint64_t> ignored_count_{0};
};

#endif // BASE_PORT_H