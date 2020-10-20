#ifndef RE_COMMON_ZMQ_PROTOREQUESTER_H
#define RE_COMMON_ZMQ_PROTOREQUESTER_H

#include <mutex>
#include <string>
#include <functional>
#include <unordered_map>
#include <list>
#include <memory>
#include <future>

#include <zmq.hpp>

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif
#include <google/protobuf/message_lite.h>
#ifdef _WIN32
#pragma warning(pop)
#endif
#include "../zmqutils.hpp"
#include "../protoregister/protoregister.hpp"


namespace zmq{
    class ProtoRequester{
        private:
            struct RequestStruct;
        public:
            ProtoRequester(std::string  address);
            ~ProtoRequester();

            template<class RequestType, class ReplyType>
            std::future<std::unique_ptr<ReplyType> > SendRequest(const std::string& function_name, const RequestType& request, const int timeout_ms);
        private:
            std::future<std::unique_ptr<google::protobuf::MessageLite> > EnqueueRequest(const std::string& fn_signature, const google::protobuf::MessageLite& request_proto, const int timeout_ms);
            std::unique_ptr<zmq::socket_t> GetRequestSocket();
            void ProcessRequests();
            void ProcessRequest(zmq::socket_t& socket, RequestStruct& request);

            struct RequestStruct{
                const std::string fn_signature;
                const std::string type_name;
                const std::string data;
                const std::chrono::milliseconds timeout; 
                std::promise<std::unique_ptr<google::protobuf::MessageLite>> promise;
            };

            const std::string connect_address_;
            ProtoRegister proto_register_;

            std::mutex request_mutex_;
            std::condition_variable request_cv_;

            std::mutex future_mutex_;
            std::future<void> requester_future_;

            std::mutex zmq_mutex_;
            std::unique_ptr<zmq::context_t> context_;
            std::list<std::unique_ptr<RequestStruct> > request_queue_;
    };
}

// REVIEW (Mitch): In-depth review required for this function...
template<class RequestType, class ReplyType>
std::future<std::unique_ptr<ReplyType> > zmq::ProtoRequester::SendRequest(const std::string& function_name, const RequestType& request, const int timeout_ms){
    static_assert(std::is_base_of<google::protobuf::MessageLite, RequestType>::value, "RequestType must inherit from google::protobuf::MessageLite");
    static_assert(std::is_base_of<google::protobuf::MessageLite, ReplyType>::value, "ReplyType must inherit from google::protobuf::MessageLite");

    //Register the callbacks
    proto_register_.RegisterProto<RequestType>();
    proto_register_.RegisterProto<ReplyType>();
    
    //Get the function signature
    const auto& fn_signature = zmq::GetFunctionSignature<RequestType, ReplyType>(function_name);

    auto future = EnqueueRequest(fn_signature, request, timeout_ms);

    //Do the up casting
    return std::async(std::launch::deferred, [timeout_ms](std::future<std::unique_ptr<google::protobuf::MessageLite> > future) {
        if(future.valid()){
            auto status = future.wait_for(std::chrono::milliseconds(timeout_ms));

            if(status == std::future_status::ready){
                try{
                    auto reply = future.get();
                    if(reply){
                        auto reply_proto_ptr = dynamic_cast<ReplyType*>(reply.get());
                        if(reply_proto_ptr){
                            //release the unique pointer
                            reply.release();
                            //Upcast and return 
                            return std::unique_ptr<ReplyType>(reply_proto_ptr);
                        }else{
                            throw std::runtime_error("Got Invalid ProtoType: " + reply->GetTypeName());
                        }
                    }
                }catch(const std::future_error& ex){
                    throw std::runtime_error(std::string("ProtoRequester destroyed with pending requests: ") + ex.what());
                }
            }else{
                throw zmq::TimeoutException("Request timed out in queue");
            }
        }
        throw std::runtime_error("Invalid Future");
    }, std::move(future));
}

#endif //RE_COMMON_ZMQ_PROTOREQUESTER_H
