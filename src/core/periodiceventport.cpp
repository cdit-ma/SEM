#include "periodiceventport.h"
#include "modellogger.h"
#include "component.h"
#include <iostream>


PeriodicEventPort::PeriodicEventPort(Component* component, std::string name, std::function<void(BaseMessage*)> callback, int milliseconds):
::InEventPort<BaseMessage>(component, name, callback, "periodic"){
    //Force set the kind
    SetKind(EventPort::Kind::PE);
    SetMaxQueueSize(1);
    this->duration_ = std::chrono::milliseconds(milliseconds);
};

void PeriodicEventPort::SetFrequency(double hz){
    if(hz > 0){
        int milliseconds = 1000.0 / hz;
        SetDuration(milliseconds);
    }
}

void PeriodicEventPort::SetDuration(int milliseconds){
    if(milliseconds >= 0){
        std::unique_lock<std::mutex> lock(mutex_);
        this->duration_ = std::chrono::milliseconds(milliseconds);
    }
}

bool PeriodicEventPort::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<BaseMessage>::Passivate()){
        if(tick_thread_){
            std::unique_lock<std::mutex> lock(mutex_);
            terminate_ = true;
        }
        return true;
    }
    return false;
}

void PeriodicEventPort::Loop(){
    StartupFinished();
    //Wait for the port to be activated before starting!
    WaitForActivate();
    //Log the port becoming online
    EventPort::LogActivation();
    
    while(true){
        EnqueueMessage(new BaseMessage());
        std::unique_lock<std::mutex> lock(mutex_);
        auto got_tick = !lock_condition_.wait_for(lock, duration_, [this]{return terminate_;});
        if(!got_tick){
            break;
        }
    }
    EventPort::LogPassivation();
}


void PeriodicEventPort::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(attributes.count("frequency")){
        SetFrequency(attributes["frequency"]->get_Double());
    }

    if(!tick_thread_){
        tick_thread_ = new std::thread(&PeriodicEventPort::Loop, this);
        //Block until our Tick thread is ready
        WaitForStartup();
    }
};

bool PeriodicEventPort::Teardown(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(::InEventPort<BaseMessage>::Teardown()){
        if(tick_thread_){
            //Join our zmq_thread
            tick_thread_->join();
            delete tick_thread_;
            tick_thread_ = 0;
        }
        return true;
    }
    return false;
};