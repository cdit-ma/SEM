#ifndef OUTEVENTPORT_HPP
#define OUTEVENTPORT_HPP

#include "../eventport.h"

//Interface for a standard templated OutEventPort
template <class T> class OutEventPort: public EventPort{
    public:
        virtual void tx_(T*) = 0;
};

#endif //OUTEVENTPORT_HPP