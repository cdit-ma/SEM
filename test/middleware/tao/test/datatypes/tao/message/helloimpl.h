
#ifndef HELLOIMPL_H
#define HELLOIMPL_H

#include "base/message/message.h"
#include <core/ports/translator.h>
#include "messageS.h"



#include <middleware/tao/requestreply/replierport.hpp>



class Hello : public virtual POA_Test::Hello{
    public:
        Hello(tao::ReplierPort<void, void, ::Base::Message, ::Test::Message, ::Hello>& port):
            eventport(port){
                std::cerr << __func__ << std::endl;
        };
        ~Hello(){};
    void send(const Test::Message& message){
        std::cerr << "TRYING TO LISTEN" << std::endl;
        auto base_message = Base::Translator<::Base::Message, ::Test::Message>::MiddlewareToBase(message);
        eventport.ProcessRequest(*base_message);
        return;
    };
    private:
        tao::ReplierPort<void, void, ::Base::Message, ::Test::Message, ::Hello>& eventport;
};



#endif //TAO_HELLOIMPL_H