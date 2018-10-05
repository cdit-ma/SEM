#ifndef ENVIRONMENTREQESTER_HEARTBEATER_H
#define ENVIRONMENTREQESTER_HEARTBEATER_H

#include <zmq/protorequester/protorequester.hpp>
#include <proto/controlmessage/controlmessage.pb.h>

class Heartbeater{
    public:
        Heartbeater(const int heartbeat_period, zmq::ProtoRequester& requester);
        ~Heartbeater();
        void Start();
        void Terminate();
        void AddCallback(std::function<void (NodeManager::EnvironmentMessage& environment_message)> callback_func);
        void SetTimeoutCallback(std::function<void (void)> timeout_callback);
        

    private:
        void HeartbeatLoop();
        void HandleReply(NodeManager::EnvironmentMessage& environment_message);


        std::future<void> heartbeat_future_;
        zmq::ProtoRequester& requester_;
        std::condition_variable heartbeat_cv_;
        std::mutex heartbeat_lock_;

        int heartbeat_period_;

        bool end_flag_ = false;

        std::function<void (NodeManager::EnvironmentMessage&)> callback_func_;
        std::function<void (void)> timeout_callback_;
};

#endif //ENVIRONMENTREQESTER_HEARTBEATER_H