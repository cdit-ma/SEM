

#ifndef HELLOCLIENT_H
#define HELLOCLIENT_H

#include <middleware/tao/requestreply/requesterport.hpp>
#include "messageC.h"
#include "base/message/message.h"
#include <core/ports/translator.h>

namespace tao{
    template <> 
    void RequesterPort<void, void, Base::Message, Test::Message, Test::Hello>::ProcessRequest(const Base::Message& message, std::chrono::milliseconds timeout){
        std::lock_guard<std::mutex> lock(client_mutex_); 

        const auto orb_endpoint = orb_endpoint_->String();
        const auto publisher_name = publisher_name_->String();
        try{
            auto& helper = tao::TaoHelper::get_tao_helper();
            auto orb = helper.get_orb(orb_endpoint); 
            
            std::cerr << "TRYING TO RESOLVE: '" << publisher_name << "'" << std::endl;
            auto ptr = helper.resolve_initial_references(orb, publisher_name);
            std::cerr << "GOT POINTER " << ptr << std::endl;

            //Another interesting TAO feature is the support for _unchecked_narrow(). This is part of the CORBA Messaging specification and essentially performs the same work as _narrow(), but it does not check the types remotely. If you have compile time knowledge that ensures the correctness of the narrow operation, it is more efficient to use the unchecked version.
            auto client = Test::Hello::_narrow(ptr);
            if(client){
                auto request_ptr = Base::Translator<Base::Message, Test::Message>::BaseToMiddleware(message);
                client->send(*request_ptr);
                std::cerr << "SENDING?" << std::endl;
                CORBA::release(client);
            }
        }catch(const CORBA::Exception& e){
            e._tao_print_exception ("Exception caught in test1:");
            std::cerr << "TX Exception:" << e._rep_id() << " " << e._name() << " " << e._info() << std::endl;
        }
    }
};

#endif //HELLOCLIENT_H