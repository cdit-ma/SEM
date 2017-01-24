#ifndef ZMQMASTER_H
#define ZMQMASTER_H

#include <google/protobuf/message_lite.h>
#include "zmq.hpp"
#include "executionmanager.h"

#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>

class ZMQMaster{
    public:
        ZMQMaster(std::string host_name, std::string endpoint, std::string graphml_path);
        ~ZMQMaster();

        void send_action(std::string node_name, google::protobuf::MessageLite* message);
        void send_action(std::string node_name, std::string action);
        
        bool action_writer_active();

    private:
        void registration_loop();
        void writer_loop();

        bool terminating = false;
        bool writer_active = false;

        std::string host_name;
        std::string endpoint_;
        std::vector<std::string> slaves_;

        std::mutex queue_mutex_;
        std::condition_variable queue_lock_condition_;
        std::queue<std::pair<std::string, std::string> > message_queue_;
        
        std::thread* registration_thread_ = 0;
        std::thread* writer_thread_ = 0;
        zmq::context_t* context_ = 0;

        ExecutionManager* execution_manager_;
};

#endif //ZMQMASTER_H