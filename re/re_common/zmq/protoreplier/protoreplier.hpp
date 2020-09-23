#ifndef RE_COMMON_ZMQ_PROTOREPLIER_H
#define RE_COMMON_ZMQ_PROTOREPLIER_H

#include <mutex>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <set>
#include <future>
#include <vector>


#include <zmq.hpp>
#include <google/protobuf/message_lite.h>
#include "../protoregister/protoregister.hpp"
#include "../zmqutils.hpp"

namespace zmq{
    class ProtoReplier{
        public:
            ProtoReplier();
            ~ProtoReplier();

            void Bind(const std::string& address);
            
            std::future<void> Start(const std::vector<std::chrono::milliseconds>& retry_timeouts = {});

            void Terminate();

            template<class RequestType, class ReplyType>
            void RegisterProtoCallback(const std::string& fn_name, std::function<std::unique_ptr<ReplyType>(const RequestType&)> fn);
        private:
            void RegisterNewProto(const std::string& fn_signature, std::function<std::unique_ptr<google::protobuf::MessageLite> (const google::protobuf::MessageLite&)> callback_function);
            zmq::socket_t GetReplySocket();
            void ZmqReplier(zmq::socket_t reply_socket, const std::vector<std::chrono::milliseconds> retry_timeouts, std::promise<void> blocked_promise);
        private:
            ProtoRegister proto_register_;

            std::mutex zmq_mutex_;
            std::unique_ptr<zmq::context_t> context_;

            std::mutex address_mutex_;
            std::set<std::string> bind_addresses_;

            std::mutex callback_mutex_;
            std::unordered_map<std::string, std::function<std::unique_ptr<google::protobuf::MessageLite> (const google::protobuf::MessageLite&)> > callback_lookup_;

            std::mutex future_mutex_;
            std::future<void> future_;
    };
};

template<class RequestType, class ReplyType>
void zmq::ProtoReplier::RegisterProtoCallback(const std::string& function_name, std::function<std::unique_ptr<ReplyType>(const RequestType&)> callback_function){
    static_assert(std::is_base_of<google::protobuf::MessageLite, RequestType>::value, "RequestType must inherit from google::protobuf::MessageLite");
    static_assert(std::is_base_of<google::protobuf::MessageLite, ReplyType>::value, "ReplyType must inherit from google::protobuf::MessageLite");

    //Register the callbacks
    proto_register_.RegisterProto<RequestType>();
    proto_register_.RegisterProto<ReplyType>();

    const auto& fn_signature = zmq::GetFunctionSignature<RequestType, ReplyType>(function_name);
    
    RegisterNewProto(fn_signature, [callback_function](const google::protobuf::MessageLite& request){
        return std::move(callback_function((const RequestType&) request));
    });
}

#endif //RE_COMMON_ZMQ_PROTOREPLIER_H