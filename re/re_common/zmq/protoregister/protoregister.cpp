#include "protoregister.hpp"

void zmq::ProtoRegister::RegisterProtoConstructor(const google::protobuf::MessageLite& default_instance){
    const auto& type_name = default_instance.GetTypeName();

    std::lock_guard<std::mutex> lock(mutex_);
    if(!proto_lookup_.count(type_name)){
        //Insert a factory function which will construct a protobuf message from the array
        proto_lookup_[type_name] = [&default_instance](const void* data, int size){
            //Construct a Protobuf message
            auto proto_obj = std::unique_ptr<google::protobuf::MessageLite>(default_instance.New());
            if(proto_obj->ParseFromArray(data, size) == false){
                //Throw an exception if we cannot parse
                throw std::runtime_error("Proto Type '" + default_instance.GetTypeName() + "' cannot parse data");
            }
            return proto_obj;
        };
    }
}

std::unique_ptr<google::protobuf::MessageLite> zmq::ProtoRegister::ConstructProto(const std::string& type_name, const zmq::message_t& data){
    return ConstructProto(type_name, data.data(), data.size());
}

std::unique_ptr<google::protobuf::MessageLite> zmq::ProtoRegister::ConstructProto(const std::string& type_name, const std::string& data){
    return ConstructProto(type_name, data.c_str(), data.size());
}

std::unique_ptr<google::protobuf::MessageLite> zmq::ProtoRegister::ConstructProto(const std::string& type_name, const void* data, int size){
    std::lock_guard<std::mutex> lock(mutex_);
    try{
        return proto_lookup_.at(type_name)(data, size);
    }catch(const std::out_of_range& exception){
        throw std::runtime_error("Proto Type '" + type_name + "' Not Registered!");
    }
}