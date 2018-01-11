#ifndef ENVIRONMENT_MANAGER_BROADCASTER
#define ENVIRONMENT_MANAGER_BROADCASTER

#include <iostream>
#include <zmq.hpp>
#include <thread>
class Broadcaster{

    public:
        Broadcaster(const std::string& endpoint, const std::string& message);
        void SetMessage(const std::string& message);
        std::string GetMessage() const;
        void StartBroadcast();
        void EndBroadcast();

    private:

        void BroadcastLoop();
        zmq::message_t GetZMQMessage();

        std::thread* broadcast_loop_;

        std::string message_;
        std::string endpoint_;
        zmq::context_t* context_;

        //Default broadcast period of one second
        int broadcast_period_ = 1;

        bool terminate_flag_ = false;

};

#endif //ENVIRONMENT_MANAGER_BROADCASTER