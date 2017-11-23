#ifndef RE_COMMON_ZMQ_UTILS_HPP
#define RE_COMMON_ZMQ_UTILS_HPP

#include <zmq.hpp>
#include <google/protobuf/message_lite.h>

namespace zmq{  
    inline zmq::message_t Proto2Zmq(const google::protobuf::MessageLite &ml){
        const auto& size = ml.ByteSize();
        zmq::message_t message(size);
        ml.SerializeToArray(message.data(), size);
        return message;
    };

    inline std::string Zmq2String(const zmq::message_t& message){
        return std::string(static_cast<const char*>(message.data()), message.size());
    };
}

#endif //RE_COMMON_ZMQ_UTILS_HPP