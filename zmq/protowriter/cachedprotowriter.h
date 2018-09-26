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
 
#ifndef RE_COMMON_ZMQ_CACHEDPROTOWRITER_H
#define RE_COMMON_ZMQ_CACHEDPROTOWRITER_H

#include "protowriter.h"

#include <queue>
#include <future>
#include <mutex>
#include <condition_variable>

struct Message_Struct{
    std::string topic;
    std::string type;
    std::string data;
};

namespace zmq{
    class CachedProtoWriter : public zmq::ProtoWriter{
        public:
            CachedProtoWriter(int cache_count = 50);
            ~CachedProtoWriter();
            
            bool PushMessage(const std::string& topic, std::unique_ptr<google::protobuf::MessageLite> message);
            void Terminate();
        private:
            void WriteQueue();

            std::queue<std::unique_ptr<Message_Struct> > ReadMessagesFromFile(const std::string& file_path);

            bool WriteDelimitedTo(const std::string& topic, const google::protobuf::MessageLite& message, google::protobuf::io::ZeroCopyOutputStream* raw_output);
            bool ReadDelimitedToStr(google::protobuf::io::ZeroCopyInputStream* raw_input, std::string& topic, std::string& type, std::string& message);

            std::mutex mutex_;

            std::string temp_file_path_;

            std::future<void> writer_future_;


            int log_count_ = 0;
            int written_to_disk_count = 0;
            int cache_count_ = 0;
            
            std::queue<std::pair<std::string, std::unique_ptr<google::protobuf::MessageLite> > > write_queue_;
            
            std::mutex queue_mutex_;
            std::condition_variable queue_lock_condition_;
            //std::mutex ;
            bool writer_terminate_ = false;
            bool running = false;
    };
};

#endif //RE_COMMON_ZMQ_CACHEDPROTOWRITER_H