#ifndef EVENTPORT_H
#define EVENTPORT_H

#include "../activatable.h"
#include "../attribute.h"
#include <map>

//Forward declare
class Component;

//Interface for a standard EventPort
class EventPort: public Activatable
{
    public:
        enum class Kind{
            NONE = 0,
            TX = 1,
            RX = 2,
            PE = 3
        };

        EventPort(Component* component, std::string name, EventPort::Kind kind, std::string middleware = "");
        EventPort(std::shared_ptr<Component> component, std::string name, EventPort::Kind kind, std::string middleware = "");
        

    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();
    public:
        void LogActivation();
        void LogPassivation();

        EventPort::Kind get_kind() const;
        std::string get_middleware() const;
        std::shared_ptr<Component> get_component();
    protected:
        void SetKind(EventPort::Kind kind);
    private:
        std::shared_ptr<Component> component_;
        
        EventPort::Kind kind_ = EventPort::Kind::NONE;
        
        std::string middleware_;
};

#endif //EVENTPORT_H