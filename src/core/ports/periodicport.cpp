#include "periodicport.h"
#include "../modellogger.h"
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
//TODO: HANDLE Frequency <= 5000hz

PeriodicPort::PeriodicPort(std::weak_ptr<Component> component, const std::string& name, const CallbackWrapper<void, BaseMessage>& callback, int milliseconds)
: ::SubscriberPort<BaseMessage>(component, name, callback, "periodic"){
    //Force set the kind
    SetKind(Port::Kind::PERIODIC);
    SetMaxQueueSize(1);

    //Construct an attribute called frequency
    frequency_ = ConstructAttribute(ATTRIBUTE_TYPE::DOUBLE, "Frequency").lock();
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

void PeriodicPort::SetDuration(int milliseconds){
    auto hz = milliseconds / 1000.0;

    if(milliseconds > 0){
        SetFrequency(1000 / milliseconds);
    }else{
        SetFrequency(0);
    }
}

bool PeriodicPort::HandleTerminate(){
    //Need to passivate before we can terminate
    HandlePassivate();

    std::lock_guard<std::mutex> lock(state_mutex_);
    if(::SubscriberPort<BaseMessage>::HandleTerminate()){
        if(thread_manager_){
            delete thread_manager_;
            thread_manager_ = 0;
        }
        return true;
    }
    return false;
};

bool PeriodicPort::HandlePassivate(){
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(::SubscriberPort<BaseMessage>::HandlePassivate()){
        {
            //Wake up the threads sleep
            std::lock_guard<std::mutex> lock2(tick_mutex_);
            interupt_ = true;
            tick_condition_.notify_all();
        }

        if(thread_manager_){
            return thread_manager_->Terminate();
        }
        return true;
    }
    return false;
}


bool PeriodicPort::HandleActivate(){
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(::SubscriberPort<BaseMessage>::HandleActivate()){
        if(thread_manager_){
            return thread_manager_->Activate();
        }
        return true;
    }
    return false;
}

void PeriodicPort::Loop(){
    thread_manager_->Thread_Configured();

    //Block until we are running, or error
    if(thread_manager_->Thread_WaitForActivate()){
        thread_manager_->Thread_Activated();
        ::Port::LogActivation();
        while(true){
            {
                std::unique_lock<std::mutex> lock(tick_mutex_);
                auto frequency = frequency_ ? frequency_->get_Double() : 0.0;

                if(frequency <= 0){
                    //Sleep indefinately
                    tick_condition_.wait(lock, [this]{return interupt_;});
                }else{
                    //Get 1 second in microseconds
                    double us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(1)).count();
                    int sleep_us = std::floor(us / frequency);
                    
                    auto duration = std::chrono::microseconds(sleep_us);
                    tick_condition_.wait_for(lock, duration, [this]{return interupt_;});
                }

                if(interupt_){
                    break;
                }
            }
            EnqueueMessage(new BaseMessage());
        }
        ::Port::LogPassivation();
    }
    thread_manager_->Thread_Terminated();
}



bool PeriodicPort::HandleConfigure(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);

    //Call into our base Configure function.
    if(::SubscriberPort<BaseMessage>::HandleConfigure()){
        if(!thread_manager_){
            thread_manager_ = new ThreadManager();
            auto future = std::async(std::launch::async, &PeriodicPort::Loop, this);
            thread_manager_->SetFuture(std::move(future));
            return thread_manager_->Configure();
        }else{
            std::cerr << "Have extra thread manager" << std::endl;
        }
    }
    return false;
}
