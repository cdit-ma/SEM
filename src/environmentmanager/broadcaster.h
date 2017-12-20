#ifndef ENVIRONMENT_MANAGER_BROADCASTER
#define ENVIRONMENT_MANAGER_BROADCASTER

#include <iostream>
#include <zmq.hpp>
#include <thread>
class Broadcaster{

    public:
        Broadcaster(const std::string& endpoint);
        void SetEndpoint(const std::string& endpoint);
        std::string GetEndpoint() const;
        void StartBroadcast();
        void EndBroadcast();

    private:
        void BroadcastLoop();
        zmq::message_t GetMessage();

        std::thread* broadcast_loop_;

        std::string broadcast_port_ = "22334";

        std::string endpoint_;
        zmq::context_t* context_;

        zmq::message_t stored_message_;
        bool message_changed_;

        //Default broadcast period of one second
        int broadcast_period_ = 1;

};

#endif //ENVIRONMENT_MANAGER_BROADCASTER