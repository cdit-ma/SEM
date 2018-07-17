#include "threadmanager.h"
#include <iostream>

ThreadManager::~ThreadManager(){
    if(thread_ || future_.valid()){
        Terminate();
    }
    if(thread_){
        //Shutdown our thread
        thread_->join();
    }
    if(future_.valid()){
        //Shutdown our thread
        future_.wait();
    }
};

void ThreadManager::SetThread(std::unique_ptr<std::thread> thread){
    thread_ = std::move(thread);
};

void ThreadManager::SetFuture(std::future<void> async_future){
    future_ = std::move(async_future);
};

void ThreadManager::Thread_Configured(){
    {
        std::lock_guard<std::mutex> state_lock(state_mutex_);
        states_.insert(State::CONFIGURED);
    }
    state_condition_.notify_all();
};

void ThreadManager::Thread_Terminated(){
    {
        std::lock_guard<std::mutex> state_lock(state_mutex_);
        states_.insert(State::TERMINATED);
    }
    state_condition_.notify_all();
};

void ThreadManager::Thread_Activated(){
    {
        std::lock_guard<std::mutex> state_lock(state_mutex_);
        states_.insert(State::ACTIVE);
    }
    state_condition_.notify_all();
};

bool ThreadManager::Thread_WaitForActivate(){
    //Any Transition should wake up the activate function
    std::unique_lock<std::mutex> transition_lock(transition_mutex_);
    transition_condition_.wait(transition_lock, [this]{
        return transitions_.count(Transition::ACTIVATE) || transitions_.count(Transition::TERMINATE);
    });
    return transitions_.count(Transition::ACTIVATE);
};

bool ThreadManager::WaitForActivated(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    state_condition_.wait(state_lock, [this]{
        return states_.count(State::ACTIVE) || states_.count(State::TERMINATED);
    });
    return states_.count(State::ACTIVE);
}

void ThreadManager::Thread_WaitForTerminate(){
    //Any Transition should wake up the activate function
    std::unique_lock<std::mutex> transition_lock(transition_mutex_);
    transition_condition_.wait(transition_lock, [this]{
        return transitions_.count(Transition::TERMINATE);
    });
    transitions_.count(Transition::TERMINATE);
}

bool ThreadManager::Configure(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    state_condition_.wait(state_lock, [this]{
        return states_.count(State::CONFIGURED) || states_.count(State::TERMINATED);
    });
    return states_.count(State::CONFIGURED);
};

bool ThreadManager::Activate(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);

    if(states_.count(State::CONFIGURED)){
        {
            //Notify That Transition has happened
            std::unique_lock<std::mutex> transition_lock(transition_mutex_);
            transitions_.insert(Transition::ACTIVATE);
            transition_condition_.notify_all();
        }

        state_condition_.wait(state_lock, [this]{
            return states_.count(State::ACTIVE) || states_.count(State::TERMINATED);
        });
        return states_.count(State::CONFIGURED);
    }
    return false;
};

void ThreadManager::SetTerminate(){
    {
        //Notify That Transition has happened
        std::lock_guard<std::mutex> transition_lock(transition_mutex_);
        transitions_.insert(Transition::TERMINATE);
    }
    transition_condition_.notify_all();
}

bool ThreadManager::Terminate(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);

    {
        //Notify That Transition has happened
        std::lock_guard<std::mutex> transition_lock(transition_mutex_);
        transitions_.insert(Transition::TERMINATE);
    }
    transition_condition_.notify_all();

    state_condition_.wait(state_lock, [this]{
        return states_.count(State::TERMINATED);
    });
    
    return states_.count(State::TERMINATED);
};

bool ThreadManager::isKillable(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    return states_.count(State::ACTIVE) && !states_.count(State::TERMINATED);
}