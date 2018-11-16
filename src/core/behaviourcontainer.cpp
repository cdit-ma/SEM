#include "behaviourcontainer.h"
#include "worker.h"
#include <list>
#include <future>
#include <iostream>

BehaviourContainer::BehaviourContainer(Class c, const std::string& inst_name)
:Activatable(c){
    set_name(inst_name);
}

BehaviourContainer::~BehaviourContainer(){
    Activatable::Terminate();
    std::lock_guard<std::mutex> lock(worker_mutex_);
    workers_.clear();
}

void BehaviourContainer::HandleActivate(){
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    for(const auto& p : workers_){
        auto& worker = p.second;
        if(worker){
            worker->Activate();
        }
    }
}

void BehaviourContainer::HandlePassivate(){
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    
    for(const auto& p : workers_){
        auto& worker = p.second;
        if(worker){
            worker->Passivate(); 
        }
    }
}

void BehaviourContainer::HandleTerminate(){
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    
    for(const auto& p : workers_){
        auto& worker = p.second;
        if(worker){
            worker->Terminate();
        }
    }
}

void BehaviourContainer::HandleConfigure(){
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    
    for(const auto& p : workers_){
        auto& worker = p.second;
        if(worker){
            worker->Configure();
        }
    }
}

std::weak_ptr<Worker> BehaviourContainer::AddWorker(std::unique_ptr<Worker> worker){
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    if(worker){
        const auto& worker_name = worker->get_name();
        if(workers_.count(worker_name) == 0){
            workers_[worker_name] = std::move(worker);
            return workers_[worker_name];
        }else{
            std::cerr << "BehaviourContainer '" << get_name()  << "' already has a Worker with name '" << worker_name << "'" << std::endl;
        }
    }
    return std::weak_ptr<Worker>();
}

std::weak_ptr<Worker> BehaviourContainer::GetWorker(const std::string& worker_name){
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    if(workers_.count(worker_name)){
        return workers_[worker_name];
    }
    std::cerr << "BehaviourContainer '" << get_name() << "' doesn't have a Worker with name '" << worker_name << "'" << std::endl;
    return std::weak_ptr<Worker>();
}


std::shared_ptr<Worker> BehaviourContainer::RemoveWorker(const std::string& worker_name){
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    
    if(workers_.count(worker_name)){
        auto worker = workers_[worker_name];
        workers_.erase(worker_name);
        return worker;
    }

    std::cerr << "BehaviourContainer '" << get_name() << "' doesn't have a Worker with name '" << worker_name << "'" << std::endl;
    return std::shared_ptr<Worker>();
}