#ifndef PERIODICEVENTPORT_H
#define PERIODICEVENTPORT_H

#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>


#include "eventports/ineventport.hpp"

class Component;
class PeriodicEventPort: public ::InEventPort<BaseMessage>{
    
    public:
        PeriodicEventPort(Component* component, std::string name, std::function<void(BaseMessage*)> callback, int milliseconds = 1000);
        ~PeriodicEventPort();
        void SetFrequency(double hz);
        void SetDuration(int milliseconds);

        void Startup(std::map<std::string, ::Attribute*> attributes);
        bool Teardown();
        bool Passivate();
    private:
        void Loop();
        
        std::mutex control_mutex_;
        std::thread* tick_thread_ = 0;
        
        std::mutex mutex_;
        std::condition_variable lock_condition_;
        
        std::chrono::milliseconds duration_;
        std::shared_ptr<Attribute> frequency_;
        bool terminate_ = false;
};
#endif //PERIODICEVENTPORT_H