#ifndef NODEMANAGER_ENVIRONMENTREQUESTER_H
#define NODEMANAGER_ENVIRONMENTREQUESTER_H

#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <future>

class EnvironmentRequester{

    struct Request{

        std::string request_type_;
        std::string request_data_;
        std::promise<std::string>* response_;
    };

    public:
        EnvironmentRequester(const std::string& deployment_id, const std::string& deployment_info);
        void Start();
        void End();

        int GetPort(const std::string& component_id, const std::string& component_info);

    private:
        std::future<std::string> SendRequest(const std::string& request_type, const std::string& request);

        std::string environment_manager_addr_;
        std::string deployment_id_;
        std::string deployment_info_;

        std::mutex clock_mutex_;
        long clock_ = 0;
        long Tick();
        long SetClock(long incoming_time);
        long GetClock();

        std::thread environment_comms_thread_;
        std::queue<Request> request_queue_;
        std::mutex request_queue_lock_;
        std::condition_variable request_queue_cv_;

};

#endif //NODEMANAGER_ENVIRONMENTREQUESTER_H
