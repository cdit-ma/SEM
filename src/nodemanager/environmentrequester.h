#ifndef NODEMANAGER_ENVIRONMENTREQUESTER_H
#define NODEMANAGER_ENVIRONMENTREQUESTER_H

#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <future>
#include <zmq.hpp>

class EnvironmentRequester{

    struct Request{

        std::string request_type_;
        std::string request_data_;
        std::promise<std::string>* response_;
    };

    struct Reply{
        std::string reply_type_;
        std::string reply_data_;
    };

    public:
        EnvironmentRequester(const std::string& manager_address,
                                const std::string& deployment_id,
                                const std::string& deployment_info);
        void Init();
        void Start();
        void HeartbeatLoop();
        void End();

        int GetPort(const std::string& component_id, const std::string& component_info);

    private:
        std::string manager_address_;
        std::string manager_endpoint_;
        std::string manager_update_endpoint_;
        std::future<std::string> QueueRequest(const std::string& request_type, const std::string& request);
        void SendRequest(Request request);

        std::string deployment_id_;
        std::string deployment_info_;

        std::mutex clock_mutex_;
        long clock_ = 0;
        long Tick();
        long SetClock(long incoming_time);
        long GetClock();

        std::thread* environment_comms_thread_;
        std::queue<Request> request_queue_;
        std::mutex request_queue_lock_;
        std::condition_variable request_queue_cv_;
        int heartbeat_period_ = 2;

        zmq::context_t* context_;
        zmq::socket_t* update_socket_;

        void ZMQSendTwoPartRequest(zmq::socket_t* socket, const std::string& request_type, const std::string& request);
        Reply ZMQReceiveTwoPartReply(zmq::socket_t* socket);
};

#endif //NODEMANAGER_ENVIRONMENTREQUESTER_H
