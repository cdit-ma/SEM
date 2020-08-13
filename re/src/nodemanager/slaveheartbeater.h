#ifndef SLAVE_HEARTBEATER_H
#define SLAVE_HEARTBEATER_H

#include "protorequester.hpp"

class SlaveHeartbeater{
    public:
        SlaveHeartbeater(zmq::ProtoRequester& requester, const std::string& ip_address, const std::string& container_id);
        void SetTimeoutCallback(std::function<void (void)> timeout_callback);

        ~SlaveHeartbeater();
    private:
        void Terminate();
        void HeartbeatLoop();

        std::future<void> heartbeat_future_;

        zmq::ProtoRequester& requester_;
        std::condition_variable heartbeat_cv_;
        std::mutex heartbeat_lock_;
        std::function<void (void)> timeout_callback_;
        bool end_flag_ = false;

        const std::chrono::milliseconds heartbeat_period_;
        const std::string& container_id_;
        const std::string& ip_address_;
};

#endif //SLAVE_HEARTBEATER_H
