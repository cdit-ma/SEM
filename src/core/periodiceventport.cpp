#include "periodiceventport.h"
#include "modellogger.h"
#include "component.h"
#include <iostream>

PeriodicEventPort::PeriodicEventPort(Component* component, std::string name, std::function<void(BaseMessage*)> callback, int milliseconds):
EventPort(component, name, EventPort::Kind::PE){
    this->callback_ = callback;
    this->duration_ = std::chrono::milliseconds(milliseconds);
}

bool PeriodicEventPort::Activate(){
    if(!is_active()){
        //Gain mutex lock and Set the terminate_tick flag
        std::unique_lock<std::mutex> lock(mutex_);
        terminate = false;
        
        //Construct a thread
        callback_thread_ = new std::thread(&PeriodicEventPort::Loop, this);
    }
    return Activatable::Activate();
}

bool PeriodicEventPort::Passivate(){
    if(is_active()){
        {
            //Gain mutex lock and Terminate, this will interupt the loop after sleep
            std::unique_lock<std::mutex> lock(mutex_);
            terminate = true;
            lock_condition_.notify_all();
        }
        callback_thread_->join();
        delete callback_thread_;
        callback_thread_ = 0;
    }
    return Activatable::Passivate();
}

bool PeriodicEventPort::WaitForTick(){
    std::unique_lock<std::mutex> lock(mutex_);
    return !lock_condition_.wait_for(lock, duration_, [&]{return terminate;});
}

void PeriodicEventPort::Loop(){
    while(true){
        if(callback_ != nullptr){
            //logger()->LogMessageEvent(this);
            //Construct a callback object
            callback_(new BaseMessage());
        }
        if(!WaitForTick()){
            break;
        }
    }
}


void PeriodicEventPort::Startup(std::map<std::string, ::Attribute*> attributes){
    if(attributes.count("frequency")){
        auto frequency = attributes["frequency"]->get_Double();
        if(frequency > 0){
            int ms = 1000.0/frequency;
            std::cout << get_component()->get_name() << "::PeriodicEventPort: " << get_name() << " Setting Frequency: " << ms << "MS"<< std::endl;
            duration_ = std::chrono::milliseconds(ms);
        }
    }
};

bool PeriodicEventPort::Teardown(){
    return true;
};