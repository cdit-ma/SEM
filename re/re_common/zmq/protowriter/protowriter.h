#ifndef RE_COMMON_ZMQ_PROTOWRITER_H
#define RE_COMMON_ZMQ_PROTOWRITER_H

#include <string>
#include <mutex>
#include <atomic>
#include <cstdint>

#include <google/protobuf/message_lite.h>
#include <zmq.hpp>

#include "monitor.h"
namespace zmq{
    class ProtoWriter{
        public:
            ProtoWriter();
            virtual ~ProtoWriter();

            uint64_t GetTxCount() const;
            
            void RegisterMonitorCallback(const uint8_t& event, std::function<void(int, std::string)> fn);

            void AttachMonitor(std::unique_ptr<zmq::Monitor> monitor, const int event_type);
            bool BindPublisherSocket(const std::string& endpoint);

            virtual bool PushMessage(const std::string& topic, std::unique_ptr<google::protobuf::MessageLite> message);
            bool PushMessage(std::unique_ptr<google::protobuf::MessageLite> message);
            
            virtual void Terminate();
        protected:
            bool PushString(const std::string& topic, const std::string& message_type, const std::string& message);
        private:
            void UpdateBackpressure(bool increment);

            std::atomic<uint64_t> tx_count_{0};
            std::atomic<int64_t> backpressure_{0};
            std::chrono::steady_clock::time_point backpressure_update_time_;
            const std::chrono::microseconds message_process_delay_;

            std::mutex mutex_;
            std::unique_ptr<zmq::socket_t> socket_;
            std::unique_ptr<zmq::context_t> context_;
            std::unique_ptr<zmq::Monitor> monitor_;
            
    };
};

#endif //RE_COMMON_ZMQ_PROTOWRITER_H