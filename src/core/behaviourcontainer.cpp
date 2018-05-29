#include "behaviourcontainer.h"
#include "worker.h"
#include <list>
#include <future>

BehaviourContainer::BehaviourContainer(const std::string& inst_name){
    set_name(inst_name);
}

BehaviourContainer::~BehaviourContainer(){
    Activatable::Terminate();
    std::lock_guard<std::mutex> lock(state_mutex_);
    workers_.clear();
}

bool BehaviourContainer::HandleActivate(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    
    for(const auto& p : workers_){
        auto& a = p.second;
        if(a){
            a->Activate();
        }
    }
    return true;
}

bool BehaviourContainer::HandlePassivate(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    
    for(const auto& p : workers_){
        auto& a = p.second;
        if(a){
            a->Passivate();
        }
    }
    return true;
}

bool BehaviourContainer::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    
    
    auto success = true;
    std::list<std::future<bool> > results;

    //Construct a list of async terminate requests.
    //This should lessen the impact of slow destruction of the ports, allowing them to be concurrent
    {
        std::lock_guard<std::mutex> worker_lock(worker_mutex_);

        for(const auto& p : workers_){
            auto& a = p.second;
            if(a){
                //Construct a thread to run the terminate function, which is blocking
                results.push_back(std::async(std::launch::async, &Activatable::Terminate, a));
            }
        }
    }

    //Join all of our termination threads
    for(auto& result : results){
        success &= result.get();
    }

    return true;
}

bool BehaviourContainer::HandleConfigure(){
    std::lock_guard<std::mutex> state_lock(state_mutex_);
    std::lock_guard<std::mutex> worker_lock(worker_mutex_);
    
    for(const auto& p : workers_){
        auto& a = p.second;
        if(a){
            a->Configure();
        }
    }
    return true;
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