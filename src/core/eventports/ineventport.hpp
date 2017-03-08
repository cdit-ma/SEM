#ifndef INEVENTPORT_HPP
#define INEVENTPORT_HPP

#include <condition_variable>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

#include "../modellogger.h"
#include "../component.h"
#include "../eventport.h"
#include <iostream>

//Interface for a standard templated InEventPort
template <class T> class InEventPort: public EventPort{
    public:
        InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function);
        virtual ~InEventPort();
        bool Activate();
        bool Passivate();
    protected:
        void EnqueueMessage(T* t);
    private:
        void rx(T* t);
        void receive_loop();
    private:
        std::function<void (T*) > callback_function_;

        bool terminate_ = false;
        
        std::queue<T*> message_queue_;
        std::mutex thread_mutex_;
        std::mutex notify_mutex_;
        
        std::thread* queue_thread_ = 0;
        std::condition_variable notify_lock_condition_;
};

template <class T>
InEventPort<T>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function) : EventPort(component, name, EventPort::Type::RX){
    if(callback_function){
        callback_function_ = callback_function;
    }else{
        std::cout << "InEventPort: " << name << " has a NULL Callback Function!" << std::endl;
    }
};

template <class T>
InEventPort<T>::~InEventPort(){
    //Teardown
};

template <class T>
bool InEventPort<T>::Activate(){
    std::lock_guard<std::mutex> lock(thread_mutex_);
    if(!queue_thread_){
        queue_thread_ = new std::thread(&InEventPort<T>::receive_loop, this);
    }
    return EventPort::Activate();
};

template <class T>
bool InEventPort<T>::Passivate(){
    //Passivate first to ignore all messages
    bool passivated = EventPort::Passivate();
    std::unique_lock<std::mutex> lock(thread_mutex_);
    
    if(queue_thread_){
        std::cout << "Joining queue_thread_" << std::endl;
        {
            std::unique_lock<std::mutex> lock(notify_mutex_);
            terminate_ = true;
            notify_lock_condition_.notify_all();
        }
        
        queue_thread_->join();
        delete queue_thread_;
        queue_thread_ = 0;
        std::cout << "Joined queue_thread_" << std::endl;
    }
    return passivated;
};

template <class T>
void InEventPort<T>::rx(T* t){
    if(is_active() && callback_function_){
        logger()->LogComponentEvent(this, t, ModelLogger::ComponentEvent::STARTED_FUNC);
        callback_function_(t);
        logger()->LogComponentEvent(this, t, ModelLogger::ComponentEvent::FINISHED_FUNC);
    }else{
        //Log that didn't call back on this message
        logger()->LogComponentEvent(this, t, ModelLogger::ComponentEvent::IGNORED);
    }
    //Free memory
    delete t;
};

template <class T>
void InEventPort<T>::receive_loop(){
    std::queue<T*> queue_;
    
    bool terminate = false;
    while(true){
        {
            //Gain Mutex
            std::unique_lock<std::mutex> lock(notify_mutex_);
            
            //Check terminate flag
            if(terminate_){
                terminate = terminate_;
            }else{
                //Wait for the next notify
                notify_lock_condition_.wait(lock);
            }
            
            //Swap out the queue's and release the mutex
            message_queue_.swap(queue_);
        }

        //Process the queue
        while(!queue_.empty()){
            auto m = queue_.front();
            //If we are terminated the rx will drop the messages
            rx(m);
            queue_.pop();
        }

        //Terminate
        if(terminate){
            return;
        }
    }
};

template <class T>
void InEventPort<T>::EnqueueMessage(T* t){
    //Log Receive Time
    logger()->LogComponentEvent(this, t, ModelLogger::ComponentEvent::RECEIVED);

    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(notify_mutex_);
    message_queue_.push(t);
    notify_lock_condition_.notify_all();
};

                
#endif //INEVENTPORT_HPP