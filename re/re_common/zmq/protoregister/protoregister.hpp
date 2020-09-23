#ifndef RE_COMMON_ZMQ_PROTOREGISTER_H
#define RE_COMMON_ZMQ_PROTOREGISTER_H

#include <mutex>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <zmq.hpp>
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif
#include <google/protobuf/message_lite.h>
#ifdef _WIN32
#pragma warning(pop)
#endif

namespace zmq{
    class ProtoRegister{
        public:
            template<class ProtoType>
            void RegisterProto();
            std::unique_ptr<google::protobuf::MessageLite> ConstructProto(const std::string& type_name, const zmq::message_t& data);
            std::unique_ptr<google::protobuf::MessageLite> ConstructProto(const std::string& type_name, const std::string& data);
        private:
            void RegisterProtoConstructor(const google::protobuf::MessageLite& default_instance);
            std::unique_ptr<google::protobuf::MessageLite> ConstructProto(const std::string& type_name, const void* data, int size);
            
            std::mutex mutex_;
            std::unordered_map<std::string, std::function<std::unique_ptr<google::protobuf::MessageLite> (const void*, int)> > proto_lookup_;
    };
};

template<class ProtoType>
void zmq::ProtoRegister::RegisterProto(){
    static_assert(std::is_base_of<google::protobuf::MessageLite, ProtoType>::value, "ProtoType must inherit from google::protobuf::MessageLite");
    RegisterProtoConstructor(ProtoType::default_instance());
};

#endif //RE_COMMON_ZMQ_PROTOREGISTER_H
