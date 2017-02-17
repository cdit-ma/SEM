#ifndef COMPONENTS_HELLOSENDER_HELLOSENDERIMPL_H
#define COMPONENTS_HELLOSENDER_HELLOSENDERIMPL_H

#include "hellosenderint.h"
class HelloSenderImpl: public HelloSenderInt{
    public:
        HelloSenderImpl(std::string name);
        void tick();
    private:
        //Variable
        int sentCount_ = 0;
};

#endif //COMPONENTS_HELLOSENDER_HELLOSENDERIMPL_H
