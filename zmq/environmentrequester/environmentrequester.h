#ifndef NODEMANAGER_ENVIRONMENTREQUESTER_H
#define NODEMANAGER_ENVIRONMENTREQUESTER_H

#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <future>

namespace NodeManager{
    class ControlMessage;
    class EnvironmentMessage;
};

namespace zmq{
    class context_t;
    class socket_t;
}

class EnvironmentRequester{
    public:
        enum class DeploymentType{
            RE_MASTER,
            RE_SLAVE,
            LOGAN
        };
        EnvironmentRequester(const std::string& manager_address,
                                const std::string& deployment_id,
                                DeploymentType deployment_type
                                );
        ~EnvironmentRequester();
        void Init();
        void Init(const std::string& manager_endpoint);
        void Start();
        void End();
        NodeManager::ControlMessage AddDeployment(NodeManager::ControlMessage& control_message);

        void RemoveDeployment();
        NodeManager::ControlMessage NodeQuery(const std::string& node_endpoint);
        
        NodeManager::EnvironmentMessage GetLoganInfo(const std::string& node_ip_address);
        void SetIPAddress(const std::string& ip_address);

        void AddUpdateCallback(std::function<void (NodeManager::EnvironmentMessage& environment_message)> callback_func);

    private:
        struct Request{
            std::string request_data_;
            std::promise<std::string>* response_;
        };

        std::unique_ptr<zmq::socket_t> ConstructRequestPort();

        DeploymentType deployment_type_;

        //Constants
        const int HEARTBEAT_PERIOD = 2000;
        const int REQUEST_TIMEOUT = 3000;

        //ZMQ endpoints
        std::string manager_address_;
        std::string manager_endpoint_;
        std::string manager_update_endpoint_;

        std::string ip_address_;

        bool environment_manager_not_found_ = false;
        
        //Threads
        void HeartbeatLoop();

        std::future<void> heartbeat_future_;

        bool end_flag_ = false;

        //Request helpers
        std::future<std::string> QueueRequest(const std::string& request);
        void SendRequest(zmq::socket_t&, Request request);
        void HandleReply(NodeManager::EnvironmentMessage& message);

        std::string experiment_id_;

        //Callback
        std::function<void (NodeManager::EnvironmentMessage&)> update_callback_;

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
        std::unique_ptr<zmq::context_t> context_;
        void ZMQSendRequest(zmq::socket_t& socket, const std::string& request);
        std::string ZMQReceiveReply(zmq::socket_t& socket);
};

#endif //NODEMANAGER_ENVIRONMENTREQUESTER_H
