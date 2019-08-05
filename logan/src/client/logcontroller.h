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
#include <vector>
#include <atomic>

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
        void Start(const std::string& publisher_endpoint, double frequency, const std::vector<std::string>& processes, const bool& live_mode = false);
        void Stop();
    private:
        void InteruptLogThread();
        void LogThread(const std::string publisher_endpoint, const double frequency, const bool& live_mode, std::promise<void> startup_promise);
        void GotNewConnection(int event_type, std::string address);
        void QueueOneTimeInfo();

        SystemInfo& system_;
        const int listener_id_;
        
        std::mutex future_mutex_;
        std::future<void> logging_future_;

        std::mutex interupt_mutex_;
        std::atomic_bool interupt_{false};
        std::condition_variable interupt_condition_;
        
        std::atomic_bool send_onetime_info_{false};
};

#endif //LOGCONTROLLER_H
