#ifndef PROTO_TRANSLATER_H
#define PROTO_TRANSLATER_H

#include <core/globalinterfaces.hpp>
#include <core/eventports/translater.h>

namespace Proto{
    template <class BaseType, class ProtoType>
    class Translater: public ::Base::Translater<BaseType, ProtoType>{
        public:
            BaseType* StringToBase(const std::string& message){
                ProtoType proto_obj;
                proto_obj.ParseFromString(message);
                return MiddlewareToBase(proto_obj);
            };

            std::string BaseToString(const BaseType& message){
                std::string str_value;
                auto proto_obj = BaseToMiddleware(message);
                proto_obj->SerializeToString(&str_value);
                delete proto_obj;
                return str_value;
            };
    };
};

#endif // PROTO_TRANSLATER_H
