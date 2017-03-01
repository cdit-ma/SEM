#ifndef RE_COMMON_ZMQ_PROTORECEIVER_H
#define RE_COMMON_ZMQ_PROTORECEIVER_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <functional>
#include <map>

#include <zmq.hpp>
#include <google/protobuf/message_lite.h>

#include "../monitor/monitorable.h"

namespace zmq{
    class ProtoReceiver: public zmq::Monitorable{
        public:
            ProtoReceiver();
            ~ProtoReceiver();
            void SetBatchMode(bool on, int size);
            void AttachMonitor(zmq::Monitor* monitor, int event_type);
            void Start();
            
            void Connect(std::string address);
            void Filter(std::string topic_filter);
            void RegisterNewProto(const google::protobuf::MessageLite &ml, std::function<void(google::protobuf::MessageLite*)> fn);
        private:
            google::protobuf::MessageLite* ConstructMessage(std::string type, std::string data);
            void Connect_(std::string address);
            void Filter_(std::string topic_filter);
            void RecieverThread();
            void ProtoConvertThread();
            void MonitorThread();

            zmq::socket_t* socket_ = 0;

            int batch_size_ = 0;
            
            std::vector<std::string> addresses_;
            std::vector<std::string> filters_;

            std::thread* reciever_thread_ = 0;
            std::thread* proto_convert_thread_ = 0;

            zmq::context_t *context_ = 0;

            std::map<std::string, std::function<void(google::protobuf::MessageLite*)> > callback_lookup_;
            std::map<std::string, std::function< google::protobuf::MessageLite* ()> > proto_lookup_;

            std::condition_variable queue_lock_condition_;
            std::mutex queue_mutex_;
            std::queue<std::pair<std::string, std::string> > rx_message_queue_; 

            std::mutex address_mutex_;
            
            bool terminate_reciever_ = false;
            bool terminate_proto_convert_thread_ = false;
    };
};

#endif //RE_COMMON_ZMQ_PROTORECEIVER_H