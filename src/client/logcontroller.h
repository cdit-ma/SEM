/* logan
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "logan"
 *
 * "logan" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "logan" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <mutex>
#include <condition_variable>
#include <future>
#include <unordered_map>
#include <vector>
#include <google/protobuf/message_lite.h>

#include "systeminfo.h"

//Forward declare
namespace re_common{
    class SystemInfo;
    class SystemStatus;
};

class LogController{
    public:
        LogController();
        ~LogController();

        std::string GetSystemInfoJson();
        bool Start(const std::string& publisher_endpoint, const double& frequency, const std::vector<std::string>& processes, const bool& live_mode = false);
        bool Stop();
    private:
        void InteruptLogThread();
        void LogThread(const std::string& publisher_endpoint, const double& frequency, const std::vector<std::string>& processes, const bool& live_mode);
        void GotNewConnection(int event_type, std::string address);
        void QueueOneTimeInfo();
        
        re_common::SystemStatus* GetSystemStatus();
        re_common::SystemInfo* GetSystemInfo();
        
        SystemInfo* system_ = 0;
        
        std::mutex state_mutex_;

        std::future<void> logging_future_;
        
        std::mutex one_time_mutex_;
        bool send_onetime_info_ = false;
        
        std::mutex interupt_mutex_;
        std::condition_variable log_condition_;
        bool interupt_ = false;
        int message_id_ = 0;

        //don't send onetime info for any contained pids
        std::unordered_map<int, double> pid_updated_times_;
};

#endif //LOGCONTROLLER_H
