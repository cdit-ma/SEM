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
        std::string request_data_;
        std::promise<std::string>* response_;
    };

    public:
        EnvironmentRequester(const std::string& manager_address,
                                const std::string& deployment_id,
                                const std::string& deployment_info);
        void Init();
        void Start();
        void End();

        int GetComponentPort(const std::string& component_id, const std::string& component_info);
        int GetDeploymentMasterPort(const std::string& deployment_id);
        int GetModelLoggerPort(const std::string& deployment_id);

    private:
        //Constants
        const int heartbeat_period_ = 2;

        //ZMQ endpoints
        std::string manager_address_;
        std::string manager_endpoint_;
        std::string manager_update_endpoint_;
        
        //Threads
        void HeartbeatLoop();
        std::thread* heartbeat_thread_;
        bool end_flag_ = false;

        //Request helpers
        std::future<std::string> QueueRequest(const std::string& request);
        void SendRequest(Request request);

        std::string deployment_id_;
        std::string deployment_info_;

        //Local clock
        std::mutex clock_mutex_;
        uint64_t clock_ = 0;
        uint64_t Tick();
        uint64_t SetClock(uint64_t incoming_time);
        uint64_t GetClock();

        //Request queue
        std::mutex request_queue_lock_;
        std::condition_variable request_queue_cv_;
        std::queue<Request> request_queue_;

        //ZMQ sockets and helpers
        zmq::context_t* context_;
        zmq::socket_t* update_socket_;
        void ZMQSendRequest(zmq::socket_t* socket, const std::string& request);
        std::string ZMQReceiveReply(zmq::socket_t* socket);
};

#endif //NODEMANAGER_ENVIRONMENTREQUESTER_H
