#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "systemstatus.pb.h"
#include "systeminfo.h"
#include "re_common/zmqprotowriter/zmqmessagewriter.h"

class LogController{
    public:
        LogController(int port, double frequency, std::vector<std::string> processes, bool cached = false);
        ~LogController();

        void Terminate();
        
    private:
        void LogThread();
        void WriteThread();
        void TerminateLogger();
        void TerminateWriter();

        SystemStatus* GetSystemStatus(SystemInfo* systemInfo);


        SystemInfo* system_info_;

        std::thread* logging_thread_;
        std::thread* writer_thread_;

        zmq::context_t* context_;
        ZMQMessageWriter* writer_;

     
        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;
        std::queue<SystemStatus*> message_queue_; 
        
        
        int message_id_ = 0;

        bool cached_mode_ = false;
        int sleep_time_ = 1000;
        
        std::vector<std::string> processes_;

        //set of seen pids
        //don't send onetime info for any contained pids
        std::set<std::string> seen_hostnames_;
        std::set<std::string> seen_fs_;
        std::set<std::string> seen_if_;

        std::map<int, double> pid_updated_times_;

        bool logger_terminate_ = false;
        bool writer_terminate_ = false;
};

#endif //LOGCONTROLLER_H