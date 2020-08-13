#ifndef PROTO_TRANSLATOR_H
#define PROTO_TRANSLATOR_H

#include "ports/translator.h"

// REVIEW (Mitch): This doesn't belong in "middlewares". Move to serialization::proto
namespace Proto{
    template <class BaseType, class ProtoType>
    class Translator: public ::Base::Translator<BaseType, ProtoType>{
        public:
            static std::unique_ptr<BaseType> StringToBase(const std::string& message){
                ProtoType proto_obj;
                proto_obj.ParseFromString(message);
                return ::Base::Translator<BaseType, ProtoType>::MiddlewareToBase(proto_obj);
            };

            static std::string BaseToString(const BaseType& message){
                std::string str_value;
                auto proto_obj = ::Base::Translator<BaseType, ProtoType>::BaseToMiddleware(message);
                proto_obj->SerializeToString(&str_value);
                return str_value;
            };
    };

    template <>
    class Translator<void, void>{
        public:
            static void StringToBase(const std::string&){
                return;
            };

            static std::string BaseToString(){
                return std::string();
            };
    };
};

#endif // PROTO_TRANSLATOR_H
