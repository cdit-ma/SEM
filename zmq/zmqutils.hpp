#ifndef RE_COMMON_ZMQ_UTILS_HPP
#define RE_COMMON_ZMQ_UTILS_HPP

#include <zmq.hpp>
#include <google/protobuf/message_lite.h>

namespace zmq{
    class TimeoutException : public std::runtime_error{
        public:
            TimeoutException(const std::string& what_arg) : std::runtime_error(what_arg){};
    };

    class RMIException : public std::runtime_error{
        public:
            RMIException(const std::string& what_arg) : std::runtime_error(what_arg){};
    };

    inline zmq::message_t Proto2Zmq(const google::protobuf::MessageLite &ml){
        const auto& size = ml.ByteSize();
        zmq::message_t message(size);
        ml.SerializeToArray(message.data(), size);
        return message;
    };

    inline std::string Zmq2String(const zmq::message_t& message){
        return std::string(static_cast<const char*>(message.data()), message.size());
    };

    inline zmq::message_t String2Zmq(const std::string& str){
        return zmq::message_t(str.c_str(), str.size());
    };

    template <class RequestType, class ReplyType>
    inline std::string GetFunctionSignature(const std::string& function_name){
        static_assert(std::is_base_of<google::protobuf::MessageLite, RequestType>::value, "RequestType must inherit from google::protobuf::MessageLite");
        static_assert(std::is_base_of<google::protobuf::MessageLite, ReplyType>::value, "ReplyType must inherit from google::protobuf::MessageLite");
        
        return function_name + "(" + RequestType::default_instance().GetTypeName() + ") -> " + ReplyType::default_instance().GetTypeName();
    };

    template <class ProtoType>
    ProtoType Zmq2Proto(const zmq::message_t& message){
        ProtoType type;

        if(type.ParseFromArray(message.data(), message.size())){
            return type;
        }
        throw std::runtime_error("Cannot Parse");
    };
}

#endif //RE_COMMON_ZMQ_UTILS_HPP