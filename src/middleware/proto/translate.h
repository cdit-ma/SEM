#ifndef PROTO_CONVERT_H
#define PROTO_CONVERT_H

#include <string>
#include <core/globalinterfaces.hpp>

namespace proto{
    template <class BaseType, class ProtoType>
    EXPORT_FUNC ProtoType* translate(const BaseType& value);
    
    template <class BaseType, class ProtoType>
    EXPORT_FUNC BaseType* translate(const ProtoType& value);
    
    template <class BaseType, class ProtoType>
    EXPORT_FUNC BaseType* decode(const std::string& value){
        ProtoType proto_obj;
		proto_obj.ParseFromString(value);
		return proto::translate<BaseType, ProtoType>(proto_obj);
    };

    template <class BaseType, class ProtoType>
    EXPORT_FUNC std::string encode(const BaseType& value){
        std::string str_value;
		auto proto_obj = proto::translate<BaseType, ProtoType>(value);
		proto_obj->SerializeToString(&str_value);
		delete proto_obj;
		return str_value;
    };
};

#endif // PROTO_MESSAGE_CONVERT_H
