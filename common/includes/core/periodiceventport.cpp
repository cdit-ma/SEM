#include "periodiceventport.h"

#include "component.h"
#include <iostream>

PeriodicEventPort::PeriodicEventPort(Component* component, std::string name, std::function<void(BaseMessage*)> callback, int milliseconds){
    this->callback_ = callback;
    this->duration_ = std::chrono::milliseconds(milliseconds);
    if(component){
        set_name(name);
        component_ = component;
        component->add_event_port(this);
    }
}

bool PeriodicEventPort::activate(){
    std::cout << "PeriodicEventPort::activate()" << std::endl;
    if(!is_active()){
        //Gain mutex lock and Set the terminate_tick flag
        std::unique_lock<std::mutex> lock(mutex_);
        terminate = false;
        
        //Construct a thread
        callback_thread_ = new std::thread(&PeriodicEventPort::loop, this);
    }
    return Activatable::activate();
}

bool PeriodicEventPort::passivate(){
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
    return Activatable::passivate();
}

bool PeriodicEventPort::wait_for_tick(){
    std::unique_lock<std::mutex> lock(mutex_);
    return !lock_condition_.wait_for(lock, duration_, [&]{return terminate;});
}

void PeriodicEventPort::loop(){
    while(true){
        if(callback_ != nullptr){
            //Construct a callback object
            callback_(new BaseMessage());
        }
        if(!wait_for_tick()){
            break;
        }
    }
}


void PeriodicEventPort::startup(std::map<std::string, ::Attribute*> attributes){
    std::cout << component_->get_name() << "::PeriodicEventPort: " << get_name() << " Setting Frequency" << std::endl;

    if(attributes.count("frequency")){
        auto frequency = attributes["frequency"]->d;
        if(frequency > 0){
            int ms = 1000.0/frequency;
            std::cout << component_->get_name() << "::PeriodicEventPort: " << get_name() << " Setting Frequency: " << ms << "MS"<< std::endl;
            duration_ = std::chrono::milliseconds(ms);
        }
    }
};

void PeriodicEventPort::teardown(){
};