#ifndef PROTO_TRANSLATOR_H
#define PROTO_TRANSLATOR_H

#include <core/globalinterfaces.hpp>
#include <core/eventports/translator.h>

namespace Proto{
    template <class BaseType, class ProtoType>
    class Translator: public ::Base::Translator<BaseType, ProtoType>{
        public:
            static BaseType* StringToBase(const std::string& message){
                ProtoType proto_obj;
                proto_obj.ParseFromString(message);
                return ::Base::Translator<BaseType, ProtoType>::MiddlewareToBase(proto_obj);
            };

            static std::string BaseToString(const BaseType& message){
                std::string str_value;
                auto proto_obj = ::Base::Translator<BaseType, ProtoType>::BaseToMiddleware(message);
                proto_obj->SerializeToString(&str_value);
                delete proto_obj;
                return str_value;
            };
    };
};

#endif // PROTO_TRANSLATOR_H
