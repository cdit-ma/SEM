#include "periodiceventport.h"
#include "../modellogger.h"
#include "../component.h"
#include <iostream>
#include <memory>

PeriodicEventPort::PeriodicEventPort(std::weak_ptr<Component> component, std::string name, std::function<void(BaseMessage&)> callback, int milliseconds):
::InEventPort<BaseMessage>(component, name, callback, "periodic")
{
    //Force set the kind
    SetKind(EventPort::Kind::PE);
    SetMaxQueueSize(1);

    //Construct an attribute called frequency
    frequency_ = ConstructAttribute(ATTRIBUTE_TYPE::DOUBLE, "Frequency").lock();
    if(frequency_){
        frequency_->set_Double(0);
    }
};

PeriodicEventPort::~PeriodicEventPort(){
    Terminate();
}

void PeriodicEventPort::SetFrequency(double hz){
    //Changing the Frequency should wake the PeriodicEvent, so it can change its frequency
    std::unique_lock<std::mutex> lock(tick_mutex_);
    if(frequency_){
        frequency_->set_Double(hz);
        thread_ready_condition_.notify_all();
    }
}

void PeriodicEventPort::SetDuration(int milliseconds){
    auto hz = milliseconds / 1000.0;

    if(milliseconds > 0){
        SetFrequency(1000 / milliseconds);
    }else{
        SetFrequency(0);
    }
}

bool PeriodicEventPort::HandleTerminate(){
    //Need to passivate before we can terminate
    HandlePassivate();

    std::lock_guard<std::mutex> lock(state_mutex_);
    if(::InEventPort<BaseMessage>::HandleTerminate()){
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

bool PeriodicEventPort::HandlePassivate(){
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(::InEventPort<BaseMessage>::HandlePassivate()){
        //Notify the tick_thread to die
        std::lock_guard<std::mutex> lock(tick_mutex_);
        interupt_ = true;
        tick_condition_.notify_all();
        return true;
    }
    return false;
}

void PeriodicEventPort::Loop(){
    
    {
        //Toggle the state
        std::unique_lock<std::mutex> lock(thread_ready_mutex_);
        thread_ready_ = true;
        thread_ready_condition_.notify_all();
    }

    //Block until we are running, or error
    if(BlockUntilStateChanged(Activatable::State::RUNNING)){
        EventPort::LogActivation();
        while(true){
            {
                std::unique_lock<std::mutex> lock(tick_mutex_);
                auto frequency = frequency_ ? frequency_->get_Double() : 0.0;

                if(frequency <= 0){
                    //Sleep indefinately
                    tick_condition_.wait(lock, [this]{return interupt_;});
                }else{
                    //Get the duration in milliseconds
                    auto ms = 1000.0 / frequency;
                    auto duration = std::chrono::milliseconds((int) ms);
                    tick_condition_.wait_for(lock, duration, [this]{return interupt_;});
                }

                if(interupt_){
                    break;
                }
            }
            EnqueueMessage(new BaseMessage());
        }
        EventPort::LogPassivation();
    }
}



bool PeriodicEventPort::HandleConfigure(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);

    //Call into our base Configure function.
    if(::InEventPort<BaseMessage>::HandleConfigure()){
        //Construct a new tick_thread which runs Loop()
        if(!tick_thread_){
            std::lock_guard<std::mutex> lock(tick_mutex_);
            interupt_ = false;
            std::unique_lock<std::mutex> thread_lock(thread_ready_mutex_);
            thread_ready_ = false;
            tick_thread_ = new std::thread(&PeriodicEventPort::Loop, this);
            //Wait for the Loop to notify this
            thread_ready_condition_.wait(thread_lock, [this]{return !thread_ready_;});
            return true;
        }
    }
    return false;
}
