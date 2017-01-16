#ifndef ZMQSLAVE_H
#define ZMQSLAVE_H

#include <google/protobuf/message_lite.h>
#include "zmq.hpp"

#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>

#include "deploymentmanager.h"

class ZMQSlave{
    public:
        ZMQSlave(DeploymentManager* manager, std::string host_name, std::string port);
        ~ZMQSlave();
    private:
        void registration_loop();
        void action_subscriber_loop();
        void action_queue_loop();

        std::string master_server_address_;
        std::string host_name_;
        std::string port_;

        bool terminating = false;


        std::mutex queue_mutex_;
        std::condition_variable queue_lock_condition_;
        std::queue<std::pair<std::string, std::string> > message_queue_;


        DeploymentManager* deployment_manager_;

        std::thread* action_thread_ = 0;
        std::thread* registration_thread_ = 0;
        std::thread* reader_thread_ = 0;
        zmq::context_t* context_ = 0;
};

#endif //ZMQSLAVE_H