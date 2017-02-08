#ifndef INEVENTPORT_HPP
#define INEVENTPORT_HPP

#include <functional>
#include "../component.h"
#include "../eventport.h"
#include <iostream>
//Interface for a standard templated InEventPort
template <class T> class InEventPort: public EventPort{
    public:
        InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function):
        EventPort(component, name, EventPort::Type::RX)
        {
            if(callback_function){
                callback_function_ = callback_function;
            }else{
                std::cout << "InEventPort: " << name << " has a NULL Callback Function!" << std::endl;
            }
        };
        virtual ~InEventPort(){};
        void rx(T* t){
            if(this->is_active() && callback_function_){
                callback_function_(t);
            }
        };
    private:
        std::function<void (T*) > callback_function_;

};

#endif //INEVENTPORT_HPP