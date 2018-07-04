#ifndef BASE_PORT_PERIODIC_H
#define BASE_PORT_PERIODIC_H

#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "pubsub/subscriberport.hpp"
#include "../threadmanager.h"

class Component;

class PeriodicPort : public ::SubscriberPort<BaseMessage>{
    public:
        PeriodicPort(std::weak_ptr<Component> component, const std::string& name, const CallbackWrapper<void, BaseMessage>& callback, int milliseconds = 1000);
        ~PeriodicPort();
        void SetFrequency(double hz);
        void SetDuration(int milliseconds);
    protected:
        void HandleActivate();
        void HandleConfigure();
        void HandlePassivate();
        void HandleTerminate();
    private:
        void InterruptLoop();
        void TickLoop();
        
        std::mutex tick_mutex_;
        bool interupt_ = false;
        std::condition_variable tick_condition_;

        std::mutex thread_ready_mutex_;
        bool thread_ready_ = false;
        std::condition_variable thread_ready_condition_;
        std::shared_ptr<Attribute> frequency_;

        std::mutex thread_manager_mutex_;
        std::unique_ptr<ThreadManager> thread_manager_;
};
#endif //BASE_PORT_PERIODIC_H