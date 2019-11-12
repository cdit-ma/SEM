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

void ThreadManager::SetState(State state){
    {
        std::lock_guard<std::mutex> state_lock(state_mutex_);
        states_.insert(state);
    }
    state_condition_.notify_all();
}

bool ThreadManager::SetTransition(Transition transition){
    bool valid_transition = true;
    switch(transition){
        case Transition::ACTIVATE:{
            std::lock_guard<std::mutex> state_lock(state_mutex_);
            // REVIEW (Mitch): use more explicit bool conversion, confusing...
            valid_transition = states_.count(State::CONFIGURED);
            break;
        }
        default:
            break;
    }

    if(valid_transition){
        {
            std::lock_guard<std::mutex> transition_lock(transition_mutex_);
            transitions_.insert(transition);
        }
        transition_condition_.notify_all();
    }
    return valid_transition;
}

void ThreadManager::SetThread(std::unique_ptr<std::thread> thread){
    thread_ = std::move(thread);
};

void ThreadManager::SetFuture(std::future<void> async_future){
    future_ = std::move(async_future);
};

void ThreadManager::Thread_Configured(){
    SetState(State::CONFIGURED);
};

void ThreadManager::Thread_Terminated(){
    SetState(State::TERMINATED);
};

void ThreadManager::Thread_Activated(){
    SetState(State::ACTIVE);
};

bool ThreadManager::WaitForState(State state){
    //Terminated state should always wake up
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    state_condition_.wait(state_lock, [=]{
        return states_.count(state) || states_.count(State::TERMINATED);
    });
    return states_.count(state);
};

bool ThreadManager::WaitForTransition(Transition transition){
    //Terminate transition shoudl always wake up everything
    std::unique_lock<std::mutex> transition_lock(transition_mutex_);
    transition_condition_.wait(transition_lock, [=]{
        return transitions_.count(transition) || transitions_.count(Transition::TERMINATE);
    });
    return transitions_.count(transition);
};

bool ThreadManager::Thread_WaitForActivate(){
    return WaitForTransition(Transition::ACTIVATE);
};

void ThreadManager::Thread_WaitForTerminate(){
    WaitForTransition(Transition::TERMINATE);
}

bool ThreadManager::WaitForActivated(){
    return WaitForState(State::ACTIVE);
}

bool ThreadManager::WaitForConfigured(){
    return WaitForState(State::CONFIGURED);
};

bool ThreadManager::Activate(){
    SetTransition(Transition::ACTIVATE);
    return WaitForState(State::ACTIVE);
}

bool ThreadManager::Terminate(){
    SetTransition(Transition::TERMINATE);
    return WaitForState(State::TERMINATED);
};

void ThreadManager::SetTerminate(){
    SetTransition(Transition::TERMINATE);
}
