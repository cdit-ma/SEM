#include "protorequester.hpp"
#include <iostream>
#include <memory>
#include <utility>
#include "../zmqutils.hpp"

zmq::ProtoRequester::ProtoRequester(std::string  connect_address):
    connect_address_(std::move(connect_address)),
    context_{new zmq::context_t(1)}
{
    assert(context_);
    //assert(connect_address_.length() > 0);'
    if (connect_address_.empty()) {
        throw std::invalid_argument("Attempting to construct ProtoRequester with an empty connect address");
    }
    std::lock_guard<std::mutex> future_lock(future_mutex_);
    requester_future_ = std::async(std::launch::async, &zmq::ProtoRequester::ProcessRequests, this);
}

zmq::ProtoRequester::~ProtoRequester(){
    std::unique_ptr<zmq::context_t> context;
    {
        //We need to "deset" the context_ to wake up the condition variable in ProcessRequests, to enable shutdown
        //Before the Context object can be destoryed.
        std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
        context = std::move(context_);
    }
    request_cv_.notify_all();
    context.reset();

    //Wait for all futures to die before allowing the requester to die
    std::lock_guard<std::mutex> future_lock(future_mutex_);
    try{
        if(requester_future_.valid()){
            requester_future_.get();
        }
    }catch(const std::exception& ex){
        std::cerr << "Failed to kill all futures when destroying zmq::ProtoRequester: " << ex.what() << std::endl;
    }
}

std::future<std::unique_ptr<google::protobuf::MessageLite> > 
zmq::ProtoRequester::EnqueueRequest(const std::string& fn_signature, const google::protobuf::MessageLite& request_proto, const int timeout_ms){
    auto request = std::unique_ptr<RequestStruct>(new RequestStruct({fn_signature, request_proto.GetTypeName(), request_proto.SerializeAsString(), std::chrono::milliseconds(timeout_ms)}));
    auto result_future = request->promise.get_future();
    
    //Insert the request_future into a list of futures this object is responsible for cleaning up
    std::lock_guard<std::mutex> lock(request_mutex_);
    request_queue_.emplace_back(std::move(request));
    request_cv_.notify_all();

    //Return the result future
    return std::move(result_future);
}

std::unique_ptr<zmq::socket_t> zmq::ProtoRequester::GetRequestSocket(){
    std::unique_ptr<zmq::socket_t> socket = nullptr;
    
    {
        std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
        if(context_){
            socket = std::make_unique<zmq::socket_t>(*context_, ZMQ_REQ);
        }
    }
    
    if(socket){
        try{
            socket->setsockopt(ZMQ_LINGER, 0);
            socket->connect(connect_address_.c_str());
            return std::move(socket);
        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                throw std::runtime_error("Failed to connect to address: '" + connect_address_ + "': " + ex.what());
            }
        }
    }
    throw std::runtime_error("Got Invalid Socket");
}

void zmq::ProtoRequester::ProcessRequests(){
    auto socket = GetRequestSocket();
    
    while(socket){
        std::unique_ptr<RequestStruct> request;
        
        {
            std::unique_lock<std::mutex> lock(request_mutex_);
            request_cv_.wait(lock, [this]{
                //Wake up if the context_ has been unset or we have request to process
                std::lock_guard<std::mutex> zmq_lock(zmq_mutex_);
                return !context_ || !request_queue_.empty();
            });
            
            if(!context_){
                request_queue_.clear();
                break;
            }

            if(!request_queue_.empty()){
                request = std::move(request_queue_.front());
                request_queue_.pop_front();
            }
        }
        try{
            ProcessRequest(*socket, *request);
        }catch(const std::exception& ex){
            std::cerr << "zmq::Protorequester encountered an error while trying to process a request; spooling up a new socket. "
                      << "Exception: " << ex.what() << std::endl;
            socket = GetRequestSocket();
        }
    }
}

void zmq::ProtoRequester::ProcessRequest(zmq::socket_t& socket, RequestStruct& request){
    try{
        //Send the request
        socket.send(String2Zmq(request.fn_signature), ZMQ_SNDMORE);
        socket.send(String2Zmq(request.type_name), ZMQ_SNDMORE);
        socket.send(String2Zmq(request.data));
            
        int events = zmq::poll({{socket, 0, ZMQ_POLLIN, 0}}, (long)request.timeout.count());

        if(events > 0){
            zmq::message_t zmq_result;
            socket.recv(&zmq_result);

            //We are expecting an int32 as the result flag, so convert from void* to int*, then dereference
            uint32_t result_code = *((uint32_t*)zmq_result.data());

            if(result_code == 0){
                //Success
                zmq::message_t zmq_reply_typename;
                zmq::message_t zmq_reply_data;

                socket.recv(&zmq_reply_typename);
                socket.recv(&zmq_reply_data);

                const auto& str_reply_typename = Zmq2String(zmq_reply_typename);

                auto proto_reply = proto_register_.ConstructProto(str_reply_typename, zmq_reply_data);
                request.promise.set_value(std::move(proto_reply));
            }else{
                //Received Error from the Replier
                zmq::message_t zmq_error_data;
                socket.recv(&zmq_error_data);

                //Receiving a string'd exception
                const auto& str_exception_str = Zmq2String(zmq_error_data);
                request.promise.set_exception(std::make_exception_ptr(RMIException(str_exception_str)));
            }
        }else{
            //If we didn't receive any events, throw a timeout exception
            request.promise.set_exception(std::make_exception_ptr(TimeoutException("Server Request timed out")));
        }
    }catch(const zmq::error_t& ex){
        if(ex.num() != ETERM){
            std::runtime_error error("Got ZMQ Exception: " + std::string(ex.what()));
            request.promise.set_exception(std::make_exception_ptr(error));
            throw;
        }else{
            std::runtime_error error("ZMQ ProtoRequester was shutdown during request handling");
            request.promise.set_exception(std::make_exception_ptr(error));
        }
    }catch(const std::exception&){
        //Catch all exceptions and pass them up the to the promise
        request.promise.set_exception(std::current_exception());
    }
}
