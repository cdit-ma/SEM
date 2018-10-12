#ifndef ENVIRONMENT_MANAGER_AGGREGATIONSERVERHANDLER_H
#define ENVIRONMENT_MANAGER_AGGREGATIONSERVERHANDLER_H

#include <future>
#include <condition_variable>

#include "environment.h"
#include <proto/controlmessage/controlmessage.pb.h>
#include <zmq/protoreplier/protoreplier.hpp>
#include <zmq/protowriter/protowriter.h>

class AggregationServerHandler {
    public:
        AggregationServerHandler(EnvironmentManager::Environment& environment,
            const std::string& environment_manager_ip_address,
            const std::string& aggregation_server_ip_address,
            std::promise<std::string> port_promise);

        ~AggregationServerHandler();

        bool IsRemovable();

    private:
        void HeartbeatLoop() noexcept;


        void PublisherLoop() noexcept;
        void PushMessage(std::unique_ptr<NodeManager::EnvironmentMessage> message);

        std::unique_ptr<NodeManager::EnvironmentMessage> HandleHeartbeat(const NodeManager::EnvironmentMessage& message);

        EnvironmentManager::Environment& environment_;
        const std::string environment_manager_ip_address_;
        const std::string aggregation_server_ip_address_;

        std::mutex replier_mutex_;
        std::unique_ptr<zmq::ProtoReplier> replier_;



        std::future<void> publisher_future_;
        std::unique_ptr<zmq::ProtoWriter> publisher_;
        std::mutex update_queue_mutex_;
        std::condition_variable update_queue_condition_;
        std::queue<std::unique_ptr<NodeManager::EnvironmentMessage> > update_message_queue_;
        bool end_flag_ = false;

};


#endif //ENVIRONMENT_MANAGER_AGGREGATIONSERVERHANDLER_H
