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
 
#ifndef RE_COMMON_ZMQ_MONITORABLE_H
#define RE_COMMON_ZMQ_MONITORABLE_H

#include <mutex>
#include <string>

namespace zmq{
    class Monitor;

    class Monitorable{
        public:
            virtual bool AttachMonitor(zmq::Monitor* monitor, const int event_type) = 0;
            std::string GetNewMonitorAddress();
        private:
            int GetNewMonitorID();    
            static int count_;
            static std::mutex mutex_;
    };
}
#endif //RE_COMMON_ZMQ_MONITORABLE_H