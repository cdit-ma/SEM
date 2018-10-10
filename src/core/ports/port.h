#ifndef BASE_PORT_H
#define BASE_PORT_H

#include "../activatable.h"
#include "../basemessage.h"

//Forward declare
class Component;

enum class ThreadState{WAITING, STARTED, ACTIVE, TERMINATED};

//Interface for a standard Port
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
        
        Port(std::weak_ptr<Component> component, const std::string& port_name, const Port::Kind& port_kind, const std::string& port_middleware);
        
        Port::Kind get_kind() const;
        std::string get_middleware() const;
        std::weak_ptr<Component> get_component() const;

        int GetEventsReceived();
        int GetEventsProcessed();
        int GetEventsIgnored();
    protected:
        void HandleConfigure();
        void HandlePassivate();
        void HandleTerminate();
        void HandleActivate();
    
        void EventRecieved(const BaseMessage& message);
        void EventProcessed(const BaseMessage& message);
        void EventIgnored(const BaseMessage& message);
        void ProcessMessageException(const BaseMessage& message, const std::string& error_str);
        void ProcessGeneralException(const std::string& error_str);
        
        void SetKind(const Port::Kind& port_kind);
    private:
        std::weak_ptr<Component> component_;
        std::string port_middleware_;
        Port::Kind port_kind_ = Port::Kind::NONE;

        std::mutex mutex_;
        int received_count_ = 0;
        int processed_count_ = 0;
        int ignored_count_ = 0;
};

#endif // BASE_PORT_H