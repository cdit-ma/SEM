#ifndef BEHAVIOUR_CONTAINER_H
#define BEHAVIOUR_CONTAINER_H

#include "activatable.h"

class Worker;

class BehaviourContainer : public Activatable{
    public:
        BehaviourContainer(const std::string& inst_name = "");
        virtual ~BehaviourContainer();        

        template<class T>
        std::shared_ptr<T> AddTypedWorker(const std::string& inst_name);
        
        template<class T>
        std::shared_ptr<T> GetTypedWorker(const std::string& inst_name);

        //Worker
        std::weak_ptr<Worker> AddWorker(std::unique_ptr<Worker> worker);
        std::weak_ptr<Worker> GetWorker(const std::string& worker_name);
        std::shared_ptr<Worker> RemoveWorker(const std::string& worker_name);
    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();
    private:
        std::mutex state_mutex_;
        std::mutex worker_mutex_;

        std::unordered_map<std::string, std::shared_ptr<Worker> > workers_;
};


template<class T>
std::shared_ptr<T> BehaviourContainer::AddTypedWorker(const std::string& inst_name){
    static_assert(std::is_base_of<Worker, T>::value, "T must inherit from Worker");
    auto t_ptr = std::unique_ptr<T>(new T(*this, inst_name));
    auto t = std::dynamic_pointer_cast<T>(AddWorker(std::move(t_ptr)).lock());
    return t;
};

template<class T>
std::shared_ptr<T> BehaviourContainer::GetTypedWorker(const std::string& inst_name){
    static_assert(std::is_base_of<Worker, T>::value, "T must inherit from Worker");
    return std::dynamic_pointer_cast<T>(GetWorker(inst_name).lock());
};


#endif //BEHAVIOUR_CONTAINER_H