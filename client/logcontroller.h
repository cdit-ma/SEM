#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "systemstatus.pb.h"
#include "systeminfo.h"
#include "zmqmessagewriter.h"

class LogController{
    public:
        LogController(double frequency, std::vector<std::string> processes, bool cached = false);
        ~LogController();
        
    private:
        void LogThread();
        void WriteThread();

        std::thread* logging_thread_;
        std::thread* writer_thread_;

        SystemStatus* GetSystemStatus(SystemInfo* systemInfo);
     
        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;
        std::queue<SystemStatus*> message_queue_; 
        
        
        int message_id_;

        bool cached_mode_;
        int sleep_time_;
        
        std::vector<std::string> processes_;

        //set of seen pids
        //don't send onetime info for any contained pids
        std::set<std::string> seen_hostnames_;
        std::set<std::string> seen_fs_;
        std::set<std::string> seen_if_;


        std::map<int, double> pid_updated_times_;
        ZMQMessageWriter* writer_;

        bool terminate_;
};

#endif //LOGCONTROLLER_H