#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <mutex>
#include <condition_variable>
#include <future>
#include <vector>
#include <atomic>
#include "systeminfohandler.h"

#include "netdata/chart_json_parser.hpp"
#include "netdata/tcp_receiver.h"
#include "netdata/device_sample_aggregator.hpp"

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

        std::shared_ptr<sem::logging::netdata::tcp_receiver> netdata_receiver_;
        std::shared_ptr<sem::logging::netdata::chart_json_parser> json_parser_;
        std::shared_ptr<sem::logging::netdata::device_sample_aggregator> netdata_aggregator_;

};

#endif //LOGCONTROLLER_H
