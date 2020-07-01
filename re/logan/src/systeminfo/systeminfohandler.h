//
// Created by mitchell on 25/6/20.
//

#ifndef SEM_SYSTEMINFOHANDLER_H
#define SEM_SYSTEMINFOHANDLER_H

#include "systemevent.pb.h"
class SystemInfo;
extern SystemInfo& GetSystemInfo();
class SystemInfoBroker {
public:
    SystemInfoBroker();
    std::unique_ptr<SystemEvent::StatusEvent> GetSystemStatus(const int listener_id);
    std::unique_ptr<SystemEvent::InfoEvent> GetSystemInfo(const int listener_id);
    void Update();
    int RegisterListener();

    void ClearMonitoredProcesses();
    void MonitorProcess(const std::string& process);

private:
    SystemInfo& system_info_;
    std::mutex mutex_;
    //don't send onetime info for any contained pids
    std::unordered_map<int, std::chrono::milliseconds> pid_updated_times_;

    int listener_count_ = 0;

    //listener ID to PIDs Seen
    std::unordered_map<int, std::set<int> > pid_lookups_;

    //listener ID to Count Seen
    std::unordered_map<int, int> listener_message_count_;

    //listener ID to last updated time
    std::unordered_map<int, std::chrono::milliseconds> listener_updated_times_;
};

#endif // SEM_SYSTEMINFOHANDLER_H
