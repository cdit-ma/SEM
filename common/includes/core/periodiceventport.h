#ifndef PERIODICEVENTPORT_H
#define PERIODICEVENTPORT_H

#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "eventport.h"

class PeriodicEventPort: public EventPort{
    
    public:
        PeriodicEventPort(std::function<void(void)> callback, int milliseconds);
        bool activate();
        bool passivate();

        void startup(std::map<std::string, ::Attribute*> attributes);
        void teardown();

    private:
        bool wait_for_tick();
        void loop();
        
        std::mutex mutex_;
        std::condition_variable lock_condition_;
        bool terminate = false;

        std::function<void(void)> callback_ ;
        std::thread * callback_thread_ = 0;
        std::chrono::milliseconds duration_;    
};
#endif //PERIODICEVENTPORT_H