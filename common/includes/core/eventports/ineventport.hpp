#ifndef INEVENTPORT_HPP
#define INEVENTPORT_HPP

#include "../eventport.h"

//Interface for a standard templated InEventPort
template <class T> class InEventPort: public EventPort{
    public:
        virtual void rx_(T*) = 0;
};

#endif //INEVENTPORT_HPP