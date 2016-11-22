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
        LogController();
        void log_thread();
        void write_thread();
    private:
        SystemStatus* getSystemStatus(SystemInfo* systemInfo);
     
    std::condition_variable queueLockCondition_;
    std::mutex queueMutex_;
    std::queue<SystemStatus*> messageQueue_; 
    std::thread* loggingThread_;
    std::thread* writerThread_;
    int message_id_;


    //set of seen pids
    //don't send onetime info for any contained pids
    std::set<int> seen_pids_;
    std::set<std::string> seen_hostnames_;
    std::set<std::string> seen_fs_;
    std::set<std::string> seen_if_; 

    std::map<int, double> pid_update_times_;

    ZMQMessageWriter* writer;
};

#endif //LOGCONTROLLER_H