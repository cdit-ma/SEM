#include "periodiceventport.h"
#include "modellogger.h"
#include "component.h"
#include <iostream>

PeriodicEventPort::PeriodicEventPort(Component* component, std::string name, std::function<void(BaseMessage*)> callback, int milliseconds):
EventPort(component, name, EventPort::Kind::PE, "periodic"){
    this->callback_ = callback;
    this->duration_ = std::chrono::milliseconds(milliseconds);
}

bool PeriodicEventPort::Activate(){
    if(Activatable::Activate()){
        {
            std::unique_lock<std::mutex> lock(mutex_);
            this->terminate = false;
        }
        {
            //Construct a thread
            std::unique_lock<std::mutex> lock(thread_mutex_);
            this->callback_thread_ = new std::thread(&PeriodicEventPort::Loop, this);
        }
        return true;
    }
    return false;
}

bool PeriodicEventPort::Passivate(){
    if(Activatable::Passivate()){
        //Gain mutex lock and Terminate, this will interupt the loop after sleep
        {
            std::unique_lock<std::mutex> lock(mutex_);
            this->terminate = true;
            this->duration_ = std::chrono::milliseconds(0);
        }
        lock_condition_.notify_all();
        return true;
    }
    return false;
}

bool PeriodicEventPort::WaitForTick(){
    std::unique_lock<std::mutex> lock(mutex_);
    return !lock_condition_.wait_for(lock, duration_, [this]{return this->terminate;});
}

void PeriodicEventPort::Loop(){
    while(true){
        BaseMessage t;
        if(is_active() && callback_){
            logger()->LogComponentEvent(this, &t, ModelLogger::ComponentEvent::STARTED_FUNC);
            callback_(&t);
            logger()->LogComponentEvent(this, &t, ModelLogger::ComponentEvent::FINISHED_FUNC);
        }else{
            //Log that didn't call back on this message
            logger()->LogComponentEvent(this, &t, ModelLogger::ComponentEvent::IGNORED);
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
            //std::cout << get_component()->get_name() << "::PeriodicEventPort: '" <<  get_name() << "'" << " Frequency: " << ms << "MS"<< std::endl;
            duration_ = std::chrono::milliseconds(ms);
        }
    }
};

bool PeriodicEventPort::Teardown(){
    std::unique_lock<std::mutex> lock(thread_mutex_);
    if(callback_thread_){
        if(callback_thread_->joinable()){
            callback_thread_->join();
        }
        
        delete callback_thread_;
        callback_thread_ = 0;
    }
    return true;
};