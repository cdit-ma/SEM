#ifndef INEVENTPORT_HPP
#define INEVENTPORT_HPP

#include <functional>
#include "../component.h"
#include "../eventport.h"

//Interface for a standard templated InEventPort
template <class T> class InEventPort: public EventPort{
    public:
        InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function){
            set_name(name);
            if(component){
                //Set our Component and attach this port
                component_ = component;
                callback_function_ = callback_function;
                component_->add_event_port(this);
            }
        };
        virtual ~InEventPort(){};
        void rx(T* t){
            if(this->is_active() && callback_function_){
                callback_function_(t);
            }
        };
    private:
        Component* component_ = 0;
        std::function<void (T*) > callback_function_;

};

#endif //INEVENTPORT_HPP