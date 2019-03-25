#include "behaviourcontainer.h"
#include "worker.h"
#include <list>
#include <future>
#include <iostream>

BehaviourContainer::BehaviourContainer(Class c, const std::string& inst_name)
: Activatable(c){
    set_name(inst_name);
}

BehaviourContainer::~BehaviourContainer(){
    Activatable::Terminate();
}

void BehaviourContainer::HandleActivate(){
    boost::shared_lock<boost::shared_mutex> lock{worker_mutex_};

    for(const auto& w : workers_){
        auto& worker = w.second;
        if(worker){
            worker->Activate();
        }
    }
    logger().LogLifecycleEvent(*this, Logger::LifeCycleEvent::ACTIVATED);
}

void BehaviourContainer::HandleConfigure(){
    boost::shared_lock<boost::shared_mutex> lock{worker_mutex_};
    
    for(const auto& w : workers_){
        auto& worker = w.second;
        if(worker){
            worker->Configure();
        }
    }

    logger().LogLifecycleEvent(*this, Logger::LifeCycleEvent::CONFIGURED);
}

void BehaviourContainer::HandlePassivate(){
    boost::shared_lock<boost::shared_mutex> lock{worker_mutex_};
    
    for(const auto& w : workers_){
        auto& worker = w.second;
        if(worker){
            worker->Passivate();
        }
    }

    logger().LogLifecycleEvent(*this, Logger::LifeCycleEvent::PASSIVATED);
}

void BehaviourContainer::HandleTerminate(){
    boost::shared_lock<boost::shared_mutex> lock{worker_mutex_};
    
    for(const auto& w : workers_){
        auto& worker = w.second;
        if(worker){
            worker->Terminate();
        }
    }

    logger().LogLifecycleEvent(*this, Logger::LifeCycleEvent::TERMINATED);
}


std::weak_ptr<Worker> BehaviourContainer::AddWorker(std::unique_ptr<Worker> worker){
    if(worker){
        boost::unique_lock<boost::shared_mutex> lock{worker_mutex_};

        const auto& worker_name = worker->get_name();
        if(workers_.count(worker_name) == 0){
            return workers_.emplace(worker_name, std::move(worker)).first->second;
        }else{
            std::cerr << "BehaviourContainer '" << get_name()  << "' already has a Worker with name '" << worker_name << "'" << std::endl;
        }
    }
    return std::weak_ptr<Worker>();
}

std::weak_ptr<Worker> BehaviourContainer::GetWorker(const std::string& worker_name){
    boost::shared_lock<boost::shared_mutex> lock{worker_mutex_};
    auto worker_itt = workers_.find(worker_name);
    if(worker_itt != workers_.end()){
        return worker_itt->second;
    }
    return std::weak_ptr<Worker>();
}


std::shared_ptr<Worker> BehaviourContainer::RemoveWorker(const std::string& worker_name){
    boost::unique_lock<boost::shared_mutex> lock{worker_mutex_};

    auto worker_itt = workers_.find(worker_name);
    if(worker_itt != workers_.end()){
        auto worker = worker_itt->second;
        workers_.erase(worker_itt);
        return worker;
    }
    std::cerr << "BehaviourContainer '" << get_name() << "' doesn't have a Worker with name '" << worker_name << "'" << std::endl;
    return std::shared_ptr<Worker>();
}