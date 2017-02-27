#ifndef LOGCONTROLLER_H
#define LOGCONTROLLER_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <set>
#include <map>

//Forward declare
class SystemInfo;
class SystemStatus;
class OneTimeSystemInfo;
class ZMQMessageWriter;
namespace google{namespace protobuf{class MessageLite;};};

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

        SystemStatus* GetSystemStatus();
        OneTimeSystemInfo* GetOneTimeInfo();
        OneTimeSystemInfo* one_time_info_ = 0;

        ZMQMessageWriter* writer_;
        SystemInfo* system_info_;

        std::thread* logging_thread_;
        std::thread* writer_thread_;

        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;
        std::queue<google::protobuf::MessageLite*> message_queue_;


        int sleep_time_ = 0;

        int message_id_ = 0;

        std::vector<std::string> processes_;
        //set of seen pids
        //don't send onetime info for any contained pids
        std::map<int, double> pid_updated_times_;

        std::set<std::string> seen_hostnames_;
        std::set<std::string> seen_fs_;
        std::set<std::string> seen_if_;


        bool logger_terminate_ = false;
        bool writer_terminate_ = false;
};

#endif //LOGCONTROLLER_H
