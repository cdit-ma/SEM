#ifndef ACTIVATABLE_H
#define ACTIVATABLE_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

#include "attribute.h"
#include <boost/thread.hpp>

#include <core/loggerproxy.h>

class Activatable{
public:
    enum class Class{
        UNKNOWN,
        COMPONENT,
        PORT,
        WORKER,
    };
    enum class Transition{
        NO_TRANSITION = 0,
        CONFIGURE = 1,
        ACTIVATE = 2,
        PASSIVATE = 3,
        TERMINATE = 4,
    };

    enum class State{
        NOT_CONFIGURED = 0,
        CONFIGURED = 1,
        RUNNING = 2,
        NOT_RUNNING = 3,
    };
    static const std::string ToString(const Transition& transation);
    static const std::string ToString(const State& transation);

    public:
        Activatable(Class c = Class::UNKNOWN);
        virtual ~Activatable(){};
        
        void set_name(std::string name);
        void set_id(std::string id);
        void set_type(std::string type);

        Class get_class() const; 

        std::string get_name() const;
        std::string get_id() const;
        std::string get_type() const;

        bool process_event();
        LoggerProxy& logger() const;

        Activatable::State get_state();


        bool Configure();
        bool Activate();
        bool Passivate();
        bool Terminate();

        std::weak_ptr<Attribute> GetAttribute(const std::string& name);
        std::weak_ptr<Attribute> ConstructAttribute(const ATTRIBUTE_TYPE type, const std::string name);
    protected:
        virtual void HandleConfigure(){};
        virtual void HandleActivate(){};
        virtual void HandlePassivate(){};
        virtual void HandleTerminate(){};
        std::weak_ptr<Attribute> AddAttribute(std::unique_ptr<Attribute> attribute);
    private:
        bool transition_state(const Activatable::Transition transition);

        std::unique_ptr<LoggerProxy> logger_;
        std::string name_;
        std::string type_;
        std::string id_;
        Class class_;

        std::mutex attributes_mutex_;
        std::unordered_map<std::string, std::shared_ptr<Attribute> > attributes_;

        //boost::shared_mutex state_mutex_;
        std::mutex state_mutex_;
        Activatable::State state_ = Activatable::State::NOT_CONFIGURED;
        
        //boost::shared_mutex transition_mutex_;
        std::mutex transition_mutex_;
        Activatable::Transition transition_ = Activatable::Transition::NO_TRANSITION;

        std::mutex transitioning_mutex_;
};

#endif //ACTIVATABLE_H