#ifndef NODEMANAGER_ENVIRONMENTREQUESTER_H
#define NODEMANAGER_ENVIRONMENTREQUESTER_H

#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <future>

class EnvironmentRequester{

    class Request{

        std::string request_type_;
        std::string request_data_;
        std::promise<std::string> response_;
    };

    public:
        EnvironmentRequester(const std::string& deployment_id, const std::string& deployment_info);
        Start();
        End();

        int GetPort();

    private:
        std::future<std::string> SendRequest(Request request);

        std::string environment_manager_addr_;

        std::mutex clock_mutex_;
        long clock_ = 0;
        long Tick();
        long SetClock(long incoming_time);
        long GetClock();

        std::thread environment_comms_thread_;
        std::queue<Request> request_queue_;

};

#endif //NODEMANAGER_ENVIRONMENTREQUESTER_H
