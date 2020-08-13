#ifndef ACTIVATABLE_H
#define ACTIVATABLE_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

#include "attribute.h"
#include "boost/thread.hpp"

#include "loggerproxy.h"
#include "statemachine.h"

class Activatable{
    // REVIEW: Ultimately this should be an interface, specifying the behaviour expected of
    //  anything inheriting from activatable
public:
    // REVIEW: This enum should remain part of this interface. Activatable implementing classes
    //   must implement a get_class (probably rename to avoid ambiguity with C++ classes) method
    //   that must return a valid Class(rename) enum.

    // REVIEW (Mitch): This enum doesn't include LoganClient as a valid class type. (LoganClient in
    //  src/nodemanager inherits from Activatable)
    enum class Class{
        UNKNOWN,
        COMPONENT,
        PORT,
        WORKER,
        DEPLOYMENT_CONTAINER
    };

    public:
        Activatable(Class c = Class::UNKNOWN);
        virtual ~Activatable(){};

        // REVIEW: Const refs/string_views on these params
        void set_name(std::string name);
        // REVIEW: This will be changed in the near future to be uuid based
        void set_id(std::string id);

        // REVIEW: Resolve ambiguity with class, currently represents the (label && type) of the
        //  interface implemented by the component or message type of port
        void set_type(std::string type);

        Class get_class() const; 

        std::string get_name() const;
        std::string get_id() const;
        std::string get_type() const;

        // REVIEW (Mitch): This function is a check while it reads like an action, potential rename
        // to "can_process_event"
        bool process_event();

        // REVIEW (Mitch): This should be moved to a pure abstract function once this class is
        // changed to being an interface
        LoggerProxy& logger() const;

        StateMachine::State get_state();

        bool Configure();
        bool Activate();
        bool Passivate();
        bool Terminate();

        // REVIEW (Mitch): Attribute getter/setter/constructor moved to pure virtual
        //  Investigate using std::variant instead of own hand-rolled `Attribute` class
        std::weak_ptr<Attribute> GetAttribute(const std::string& name);
        // REVIEW (Mitch): ConstructAttribute actually tries a get before building a new attribute.
        std::weak_ptr<Attribute> ConstructAttribute(const ATTRIBUTE_TYPE type, const std::string name);

        template<class PrimitiveType>
        void SetAttributeValue(const std::string& name, const PrimitiveType& value){
            auto attr = GetAttribute(name).lock();
            if(attr){
                return attr->SetValue<PrimitiveType>(value);
            }
            throw std::invalid_argument("No attribute with name '" + name + "'");
        };

        template<class PrimitiveType>
        PrimitiveType& GetAttributeValue(const std::string& name){
            auto attr = GetAttribute(name).lock();
            if(attr){
                return attr->Value<PrimitiveType>();
            }
            throw std::invalid_argument("No attribute with name '" + name + "'");
        };
    protected:
        virtual void HandleConfigure(){};
        virtual void HandleActivate(){};
        virtual void HandlePassivate(){};
        virtual void HandleTerminate(){};
        std::weak_ptr<Attribute> AddAttribute(std::unique_ptr<Attribute> attribute);
        std::shared_ptr<Attribute> GetAttribute(const std::string& name, const ATTRIBUTE_TYPE type);
    private:
        StateMachine state_machine_;

        std::unique_ptr<LoggerProxy> logger_;
        std::string name_;
        std::string type_;
        std::string id_;
        Class class_;

        std::mutex attributes_mutex_;
        std::unordered_map<std::string, std::shared_ptr<Attribute> > attributes_;
};


#endif //ACTIVATABLE_H