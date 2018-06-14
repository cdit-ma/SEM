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

PeriodicPort::PeriodicPort(std::weak_ptr<Component> component, const std::string& name, std::function<void(BaseMessage&)> callback, int milliseconds)
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

bool PeriodicPort::HandlePassivate(){
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(::SubscriberPort<BaseMessage>::HandlePassivate()){
        //Notify the tick_thread to die
        std::lock_guard<std::mutex> lock(tick_mutex_);
        interupt_ = true;
        tick_condition_.notify_all();
        return true;
    }
    return false;
}

void PeriodicPort::Loop(){
    
    {
        //Toggle the state
        std::unique_lock<std::mutex> lock(thread_ready_mutex_);
        thread_ready_ = true;
        thread_ready_condition_.notify_all();
    }

    //Block until we are running, or error
    if(BlockUntilStateChanged(Activatable::State::RUNNING)){
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
}



bool PeriodicPort::HandleConfigure(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);

    //Call into our base Configure function.
    if(::SubscriberPort<BaseMessage>::HandleConfigure()){
        //Construct a new tick_thread which runs Loop()
        if(!tick_thread_){
            std::lock_guard<std::mutex> lock(tick_mutex_);
            interupt_ = false;
            std::unique_lock<std::mutex> thread_lock(thread_ready_mutex_);
            thread_ready_ = false;
            tick_thread_ = new std::thread(&PeriodicPort::Loop, this);
            //Wait for the Loop to notify this
            thread_ready_condition_.wait(thread_lock, [this]{return !thread_ready_;});
            return true;
        }
    }
    return false;
}
