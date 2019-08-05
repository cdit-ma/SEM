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

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <functional>
#include <unordered_map>
#include <map>
#include <future>
#include <set>

#include <zmq.hpp>
#include <google/protobuf/message_lite.h>
#include "../protoregister/protoregister.hpp"

namespace zmq{
    class ProtoReceiver{
        public:
            ProtoReceiver();
            ~ProtoReceiver();

            void Connect(const std::string& address);
            void Disconnect(const std::string& address);
            void Filter(const std::string& topic_filter);
            void Unfilter(const std::string& topic_filter);
            void SetBatchMode(size_t batch_size);
            
            
            int GetRxCount(); 
            template<class ProtoType>
            void RegisterProtoCallback(std::function<void(const ProtoType&)> fn);
        private:
            void RegisterNewProto(const google::protobuf::MessageLite& message_default_instance, std::function<void(const google::protobuf::MessageLite&)> callback_function);
            void ProcessMessage(const zmq::message_t& proto_type, const zmq::message_t& proto_data);

            void ProtoConverter();
            void ZmqReceiver(std::unique_ptr<zmq::socket_t> inproc_socket);

        private:
            std::unique_ptr<zmq::socket_t> GetSubSocket();
            std::unique_ptr<zmq::socket_t> GetInprocSocket();
            void SendInprocMessage(const std::string& function, const std::string& args);
            ProtoRegister proto_register_;
            
            const std::string inproc_address_;

            std::mutex zmq_mutex_;
            std::unique_ptr<zmq::context_t> context_;
            std::set<std::string> addresses_;
            std::set<std::string> filters_;

            std::mutex callback_mutex_;
            std::multimap<std::string, std::function<void(const google::protobuf::MessageLite&)> > callback_lookup_;

            std::mutex queue_mutex_;
            size_t batch_size_ = 0;
            std::condition_variable queue_lock_condition_;
            std::queue<std::pair<zmq::message_t, zmq::message_t> > rx_message_queue_; 
            int rx_count_ = 0;

            std::mutex future_mutex_;
            bool terminate_proto_converter_ = false;
            std::future<void> zmq_receiver_;
            std::future<void> proto_converter_;
    };
};

template<class ProtoType>
void zmq::ProtoReceiver::RegisterProtoCallback(std::function<void(const ProtoType&)> callback_function){
    static_assert(std::is_base_of<google::protobuf::MessageLite, ProtoType>::value, "ProtoType must inherit from google::protobuf::MessageLite");
    
    //Register the callbacks
    proto_register_.RegisterProto<ProtoType>();

    RegisterNewProto(ProtoType::default_instance(), [callback_function](const google::protobuf::MessageLite& ml){
        //Call into the function provided with an up-casted message
        callback_function((const ProtoType&) ml);
    });
}

#endif //RE_COMMON_ZMQ_PROTORECEIVER_H