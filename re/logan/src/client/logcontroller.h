#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include "systeminfobroker.h"
#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <vector>

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

        SystemInfoBroker system_;
        const int listener_id_;
        
        std::mutex future_mutex_;
        std::future<void> logging_future_;

        std::mutex interupt_mutex_;
        std::atomic_bool interupt_{false};
        std::condition_variable interupt_condition_;
        
        std::atomic_bool send_onetime_info_{false};
};

#endif //LOGCONTROLLER_H
