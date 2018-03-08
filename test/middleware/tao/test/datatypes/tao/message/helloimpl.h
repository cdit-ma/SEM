
#ifndef HELLOIMPL_H
#define HELLOIMPL_H

#include <middleware/tao/ineventport.hpp>
#include "messageS.h"

namespace Test{
    class HelloImpl : public POA_Test::Hello{
        public:
            HelloImpl(tao::Enqueuer<Test::Message>& eventport):
                eventport(eventport){
            };
        void send(const Test::Message& message){
            eventport.enqueue(message);
        };
        private:
            tao::Enqueuer<Test::Message> &eventport;
    };
};

#endif //TAO_HELLOIMPL_H