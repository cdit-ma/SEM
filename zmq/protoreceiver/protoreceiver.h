/* re_common
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "re_common"
 *
 * "re_common" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "re_common" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#ifndef RE_COMMON_ZMQ_PROTORECEIVER_H
#define RE_COMMON_ZMQ_PROTORECEIVER_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <functional>
#include <unordered_map>
#include <map>

#include <zmq.hpp>
#include <google/protobuf/message_lite.h>

#include "../monitor/monitorable.h"

namespace zmq{
    class ProtoReceiver: public zmq::Monitorable{
        public:
            ProtoReceiver();
            ~ProtoReceiver();

            void SetBatchMode(const bool on, const int size);
            bool AttachMonitor(zmq::Monitor* monitor, const int event_type);

            int GetRxCount(); 
            bool Start();
            bool Terminate();
            
            bool Connect(const std::string& address);
            bool Filter(const std::string& topic_filter);

            template<class T>
            bool RegisterProtoCallback(std::function<void(const T&)> fn);
        private:
            bool ProcessMessage(const zmq::message_t& type, const zmq::message_t& data);
            bool RegisterNewProto(const google::protobuf::MessageLite &ml, std::function<void(const google::protobuf::MessageLite&)> fn);
            
            void RecieverThread();
            //RecieverThread helper functions
            bool Connect_(const std::string& address);
            bool Filter_(const std::string& topic_filter);
            
            void ProtoConvertThread();

            std::mutex zmq_mutex_;
            zmq::context_t *context_ = 0;
            zmq::socket_t* socket_ = 0;
            std::vector<std::string> addresses_;
            std::vector<std::string> filters_;

            std::mutex proto_mutex_;
            std::multimap<std::string, std::function<void(const google::protobuf::MessageLite&)> > callback_lookup_;
            
            std::unordered_map<std::string, std::function<google::protobuf::MessageLite* (const zmq::message_t&)> > proto_lookup_;

            std::mutex queue_mutex_;
            int batch_size_ = 0;
            std::condition_variable queue_lock_condition_;
            std::queue<std::pair<zmq::message_t, zmq::message_t> > rx_message_queue_; 
            int rx_count_ = 0;

            std::mutex thread_mutex_;
            std::thread* reciever_thread_ = 0;
            std::thread* proto_convert_thread_ = 0;
            bool terminate_proto_convert_thread_ = false;
    };
};

template<class T>
bool zmq::ProtoReceiver::RegisterProtoCallback(std::function<void(const T&)> fn){
    static_assert(std::is_base_of<google::protobuf::MessageLite, T>::value, "T must inherit from google::protobuf::MessageLite");
    const auto& default_instance = T::default_instance();

    RegisterNewProto(default_instance, [fn](const google::protobuf::MessageLite& ml){
        auto t_message = (const T&) ml;
        fn(t_message);
    });
    return true;
}

#endif //RE_COMMON_ZMQ_PROTORECEIVER_H