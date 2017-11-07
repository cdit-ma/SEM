#include "periodiceventport.h"
#include "modellogger.h"
#include "component.h"
#include <iostream>


PeriodicEventPort::PeriodicEventPort(Component* component, std::string name, std::function<void(BaseMessage*)> callback, int milliseconds):
::InEventPort<BaseMessage>(component, name, callback, "periodic"){
    //Force set the kind
    SetKind(EventPort::Kind::PE);
    SetMaxQueueSize(1);

    frequency_ = AddAttribute(std::make_shared<Attribute>(ATTRIBUTE_TYPE::DOUBLE, "frequency"));
    frequency_->set_Double(0);
};

void PeriodicEventPort::SetFrequency(double hz){
    std::unique_lock<std::mutex> lock(mutex_);
    frequency_->set_Double(hz);
}

void PeriodicEventPort::SetDuration(int milliseconds){
    auto hz = milliseconds / 1000.0;

    if(milliseconds > 0){
        SetFrequency(1000 / milliseconds);
    }else{
        SetFrequency(0);
    }
}

bool PeriodicEventPort::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<BaseMessage>::Passivate()){
        if(tick_thread_){
            std::unique_lock<std::mutex> lock(mutex_);
            terminate_ = true;
            lock_condition_.notify_all();
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
        //Sleep first
        {
            std::unique_lock<std::mutex> lock(mutex_);
            auto frequency = frequency_->get_Double();
            if(frequency <= 0){
                //Sleep indefinately
                lock_condition_.wait(lock, [this]{return terminate_;});
            }else{
                //Get the duration in milliseconds
                auto ms = 1000.0 / frequency;
                auto duration = std::chrono::milliseconds((int) ms);
                lock_condition_.wait_for(lock, duration, [this]{return terminate_;});
            }
            if(terminate_){
                break;
            }
        }
        EnqueueMessage(new BaseMessage());
    }

    EventPort::LogPassivation();
}

PeriodicEventPort::~PeriodicEventPort(){
    //Force a passivate
    Passivate();
    //Force a teardown
    Teardown();
}

void PeriodicEventPort::Startup(std::map<std::string, ::Attribute*>){
    std::lock_guard<std::mutex> lock(control_mutex_);

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