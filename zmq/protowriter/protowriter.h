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
 
#ifndef RE_COMMON_ZMQ_PROTOWRITER_H
#define RE_COMMON_ZMQ_PROTOWRITER_H

#include <string>
#include <mutex>
#include <google/protobuf/message_lite.h>
#include <zmq.hpp>

#include "monitor.h"

namespace zmq{
    class ProtoWriter{
        public:
            ProtoWriter();
            virtual ~ProtoWriter();

            int GetTxCount();
            
            void AttachMonitor(std::unique_ptr<zmq::Monitor> monitor, const int event_type);
            bool BindPublisherSocket(const std::string& endpoint);

            virtual bool PushMessage(const std::string& topic, google::protobuf::MessageLite* message);
            bool PushMessage(google::protobuf::MessageLite* message);
            virtual void Terminate();
        protected:
            bool PushString(const std::string& topic, const std::string& message_type, const std::string& message);
        private:
            int tx_count_ = 0;

            std::unique_ptr<zmq::socket_t> socket_;
            std::unique_ptr<zmq::context_t> context_;

            std::vector< std::future<void> > monitor_futures_;
            
            std::vector< std::unique_ptr<zmq::Monitor> >  monitors_;
            std::mutex mutex_;
    };
};

#endif //RE_COMMON_ZMQ_PROTOWRITER_H