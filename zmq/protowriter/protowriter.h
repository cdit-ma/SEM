#ifndef RE_COMMON_ZMQ_PROTOWRITER_H
#define RE_COMMON_ZMQ_PROTOWRITER_H

#include <string>
#include <mutex>
#include <google/protobuf/message_lite.h>
#include <zmq.hpp>

#include "../monitor/monitorable.h"

namespace zmq{
    class ProtoWriter : public zmq::Monitorable{
        public:
            ProtoWriter();
            virtual ~ProtoWriter();
            
            void AttachMonitor(zmq::Monitor* monitor, int event_type);

            bool BindPublisherSocket(std::string endpoint);
            virtual void PushMessage(std::string topic, google::protobuf::MessageLite* message);
            virtual void Terminate();
        protected:
            void PushString(std::string* topic, std::string* message_type, std::string* message);
        private:
            zmq::socket_t* socket_;
            zmq::context_t* context_;
            std::mutex mutex_;
    };
};

#endif //RE_COMMON_ZMQ_PROTOWRITER_H