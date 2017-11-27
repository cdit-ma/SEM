#ifndef PERIODICEVENTPORT_H
#define PERIODICEVENTPORT_H

#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>


#include "ineventport.hpp"

class Component;
class PeriodicEventPort: public ::InEventPort<BaseMessage>{
    public:
        PeriodicEventPort(std::weak_ptr<Component> component, std::string name, std::function<void(BaseMessage&)> callback, int milliseconds = 1000);
        ~PeriodicEventPort();
        void SetFrequency(double hz);
        void SetDuration(int milliseconds);

    protected:
        bool HandleConfigure();
        bool HandlePassivate();
        bool HandleTerminate();
    private:
        void Loop();
        
        std::mutex state_mutex_;
        std::thread* tick_thread_ = 0;
        
        std::mutex tick_mutex_;
        bool interupt_ = false;
        std::condition_variable tick_condition_;

        std::mutex thread_ready_mutex_;
        bool thread_ready_ = false;
        std::condition_variable thread_ready_condition_;
        std::shared_ptr<Attribute> frequency_;
};
#endif //PERIODICEVENTPORT_H