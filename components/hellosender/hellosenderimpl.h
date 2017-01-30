#ifndef COMPONENTS_HELLOSENDER_HELLOSENDERIMPL_H
#define COMPONENTS_HELLOSENDER_HELLOSENDERIMPL_H

#include "hellosenderint.h"

class HelloSenderImpl: public HelloSenderInt{
    public:
        HelloSenderImpl(std::string name);

        static Component* constructor_fn(std::string name);
        static void destructor_fn(Component* c);

        void periodic_event();
    private:
        //Variable
        int sentCount_ = 0;
};


extern "C" Component* construct_component(std::string name)
{
    return new HelloSenderImpl(name);
};


#endif //COMPONENTS_HELLOSENDER_HELLOSENDERIMPL_H
