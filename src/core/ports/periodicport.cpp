#include "periodicport.h"
#include "../component.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <cmath>

/*
//Average Statistics over 1 Minute
1hz: 0.983333hz
10hz: 9.66667hz
100hz: 88.7167hz
1000hz: 911.883hz
2000hz: 1791.47hz
5000hz: 3895.85hz
10000hz: 7545.08hz
*/

//TODO: LOOKUP Leaky bucket model

PeriodicPort::PeriodicPort(std::weak_ptr<Component> component, const std::string& name, const CallbackWrapper<void, BaseMessage>& callback, int milliseconds)
: ::SubscriberPort<base_type>(component, name, callback, "periodic"){
    //Force set the kind
    SetKind(Port::Kind::PERIODIC);
    SetMaxQueueSize(1);
    set_type("periodic");

    //Construct an attribute called frequency
    frequency_ = ConstructAttribute(ATTRIBUTE_TYPE::DOUBLE, "frequency").lock();
    if(frequency_){
        frequency_->set_Double(0);
    }
};

PeriodicPort::~PeriodicPort(){
    Terminate();
}

void PeriodicPort::SetFrequency(double hz){
    //Changing the Frequency should wake the PeriodicEvent, so it can change its frequency
    std::unique_lock<std::mutex> lock(tick_mutex_);
    if(frequency_){
        frequency_->set_Double(hz);
        thread_ready_condition_.notify_all();
    }
}

double PeriodicPort::GetFrequency() const{
    if(frequency_){
        return frequency_->get_Double();
    }
    return -1;
}

void PeriodicPort::SetDuration(int milliseconds){
    auto hz = milliseconds / 1000.0;

    if(milliseconds > 0){
        SetFrequency(1000 / milliseconds);
    }else{
        SetFrequency(0);
    }
}

void PeriodicPort::HandleConfigure(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(!thread_manager_){
        thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());

        auto future = std::async(std::launch::async, &PeriodicPort::TickLoop, this);
        thread_manager_->SetFuture(std::move(future));
        thread_manager_->WaitForConfigured();
    }else{
        throw std::runtime_error("PeriodicPort has an active ThreadManager");
    }
    ::SubscriberPort<base_type>::HandleConfigure();
}

void PeriodicPort::HandleActivate(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_){
        thread_manager_->Activate();
    }else{
        throw std::runtime_error("PeriodicPort has no Thread Manager");
    }
    ::SubscriberPort<base_type>::HandleActivate();
}

void PeriodicPort::HandlePassivate(){
    InterruptLoop();
    ::SubscriberPort<base_type>::HandlePassivate();
}

void PeriodicPort::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_){
        thread_manager_->Terminate();
        thread_manager_.reset();
    }
    ::SubscriberPort<base_type>::HandleTerminate();
};

void PeriodicPort::InterruptLoop(){
    {
        //Wake up the threads sleep
        std::lock_guard<std::mutex> lock(tick_mutex_);
        interupt_ = true;
    }
    tick_condition_.notify_all();
}


void PeriodicPort::TickLoop(){
    thread_manager_->Thread_Configured();

    //Block until we are running, or error
    if(thread_manager_->Thread_WaitForActivate()){
        thread_manager_->Thread_Activated();
        while(true){
            {
                std::unique_lock<std::mutex> lock(tick_mutex_);
                auto frequency = frequency_ ? frequency_->get_Double() : 0.0;

                if(frequency <= 0){
                    //Sleep indefinately
                    tick_condition_.wait(lock, [this]{return interupt_;});
                }else{
                    //Get 1 second in microseconds
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(1)) / frequency;
                    tick_condition_.wait_for(lock, duration, [this]{return interupt_;});
                }

                if(interupt_){
                    break;
                }
            }

            auto base_type_ptr = std::unique_ptr<base_type>(new BaseMessage());
            this->EnqueueMessage(std::move(base_type_ptr));
        }
    }
    
    thread_manager_->Thread_Terminated();
}


