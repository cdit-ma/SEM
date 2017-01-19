#include "periodiceventport.h"

#include "component.h"
#include <iostream>

PeriodicEventPort::PeriodicEventPort(Component* component, std::string name, std::function<void(void)> callback, int milliseconds){
    this->callback_ = callback;
    this->duration_ = std::chrono::milliseconds(milliseconds);
    if(component){
        set_name(name);
        component_ = component;
        component->add_event_port(this);
    }
}

bool PeriodicEventPort::activate(){
    if(!is_active()){
        //Gain mutex lock and Set the terminate_tick flag
        std::unique_lock<std::mutex> lock(mutex_);
        terminate = false;
        
        //Construct a thread
        callback_thread_ = new std::thread(&PeriodicEventPort::loop, this);
        std::cout << this << "Call Back Thread Constructed:" << callback_thread_ << std::endl;
        Activatable::activate();
    }
    return true;
}

bool PeriodicEventPort::passivate(){
    if(is_active()){
        {
            //Gain mutex lock and Terminate, this will interupt the loop after sleep
            std::unique_lock<std::mutex> lock(mutex_);
            terminate = true;
            std::cout << "TERMINATING!" << std::endl;
            lock_condition_.notify_all();
        }
        callback_thread_->join();
        delete callback_thread_;
        callback_thread_ = 0;
        Activatable::passivate();
    }
    return true;
}

bool PeriodicEventPort::wait_for_tick(){
    std::unique_lock<std::mutex> lock(mutex_);
    return !lock_condition_.wait_for(lock, duration_, [&]{return terminate;});
}

void PeriodicEventPort::loop(){
    while(true){
        if(callback_ != nullptr){
            callback_();
        }
        if(!wait_for_tick()){
            std::cout << "TERMINATING out of loop!" << std::endl;
            break;
        }
    }
}


void PeriodicEventPort::startup(std::map<std::string, ::Attribute*> attributes){
    
};

void PeriodicEventPort::teardown(){
};