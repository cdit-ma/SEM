#ifndef INEVENTPORT_HPP
#define INEVENTPORT_HPP

#include <condition_variable>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>


#include "eventport.h"

#include "../modellogger.h"
#include "../component.h"
#include <iostream>

//Interface for a standard templated InEventPort
template <class T> class InEventPort: public EventPort{
    public:
        InEventPort(std::shared_ptr<Component> component, std::string name, std::function<void (T*) > callback_function, std::string middleware);
        ~InEventPort();
        void SetMaxQueueSize(int max_queue_size);
        int GetEventsReceieved();
        int GetEventsProcessed();
    protected:
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();

        void EnqueueMessage(T* t);
        int GetQueuedMessageCount();
    private:
        bool rx(T* t);
        void receive_loop();
    private:
        std::function<void (T*) > callback_function_;

        
        std::mutex queue_mutex_;
        
        //Queue Mutex responsible for these Variables
        std::queue<T*> message_queue_;
        int max_queue_size_ = -1;
        int process_queue_size_ = 0;
        

        int rx_count = 0;
        int rx_proc_count = 0;
        
        std::mutex notify_mutex_;
        bool terminate_ = false;
        std::condition_variable notify_lock_condition_;

        
        std::mutex rx_setup_mutex_;
        bool rx_setup_ = false;
        std::condition_variable rx_setup_condition_;

        std::mutex control_mutex_;
        //Normal Mutex is for changing properties 
        std::thread* queue_thread_ = 0;
};

template <class T>
InEventPort<T>::InEventPort(std::shared_ptr<Component> component, std::string name, std::function<void (T*) > callback_function, std::string middleware)
:EventPort(component, name, EventPort::Kind::RX, middleware){
    if(callback_function){
        callback_function_ = callback_function;
    }else{
        std::cerr << "InEventPort: " << name << " has a NULL Callback Function!" << std::endl;
    }
};

template <class T>
InEventPort<T>::~InEventPort(){
    //Activatable::Terminate();
};

template <class T>
void InEventPort<T>::SetMaxQueueSize(int max_queue_size){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    max_queue_size_ = max_queue_size;
};

template <class T>
int InEventPort<T>::GetEventsReceieved(){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return rx_count;
};

template <class T>
int InEventPort<T>::GetEventsProcessed(){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return rx_proc_count;
};

template <class T>
bool InEventPort<T>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(EventPort::HandlePassivate()){
        if(queue_thread_){
            //Gain the notify_mutex to flag the terminate flag
            {
                std::unique_lock<std::mutex> lock(notify_mutex_);
                terminate_ = true;
            }
            notify_lock_condition_.notify_all();
        }
        return true;
    }
    return false;
};

template <class T>
bool InEventPort<T>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(EventPort::HandleConfigure()){
        if(!queue_thread_){
            queue_thread_ = new std::thread(&InEventPort<T>::receive_loop, this);
            std::unique_lock<std::mutex> lock2(rx_setup_mutex_);
            rx_setup_condition_.wait(lock2, [=]{return rx_setup_;});
            return true;
        }
    }
    return false;
};


template <class T>
bool InEventPort<T>::HandleTerminate(){
    InEventPort<T>::HandlePassivate();
    std::unique_lock<std::mutex> lock(control_mutex_);
    if(EventPort::HandleTerminate()){
        if(queue_thread_){
            queue_thread_->join();
            delete queue_thread_;
            queue_thread_ = 0;
        }
        return true;
    }
    return false;
};

template <class T>
bool InEventPort<T>::rx(T* t){
    bool rx_d = false;
    if(is_running() && callback_function_){

        //logger()->LogComponentEvent(*this, t, ModelLogger::ComponentEvent::STARTED_FUNC);
        callback_function_(t);
        //logger()->LogComponentEvent(*this, t, ModelLogger::ComponentEvent::FINISHED_FUNC);
        rx_d = true;
    }else{
        //Log that didn't call back on this message
        //logger()->LogComponentEvent(*this, t, ModelLogger::ComponentEvent::IGNORED);
    }
    if(t){
        //Free memory
        delete t;
    }
    return rx_d;
};

template <class T>
void InEventPort<T>::receive_loop(){
    //Change the state to be Configured
    {
        std::lock_guard<std::mutex> lock(rx_setup_mutex_);
        rx_setup_ = true;
        rx_setup_condition_.notify_all();
        
    }

    std::queue<T*> queue_;
    int count = 0;
    
    bool terminate = false;
    while(true){
        {
            //Gain Mutex
            std::unique_lock<std::mutex> notify_lock(notify_mutex_);
            notify_lock_condition_.wait(notify_lock, [this]{
                if(!terminate_){
                    std::unique_lock<std::mutex> queue_lock(queue_mutex_);

                    return message_queue_.size() != 0;
                }
                return terminate_;
            });

            //Gain Mutex
            std::unique_lock<std::mutex> queue_lock(queue_mutex_);
            if(message_queue_.size()){
                //Swap out the queue's and release the mutex
                message_queue_.swap(queue_);
                process_queue_size_ = queue_.size();
            }
        }

        //Process the queue
        while(!queue_.empty()){
            auto m = queue_.front();
            auto success = rx(m);
            queue_.pop();
            
            //Gain Mutex to update the process queue size and our rx count

            std::unique_lock<std::mutex> lock(queue_mutex_);
            process_queue_size_ = queue_.size();
            if(success){
                rx_proc_count ++;
            }
        }


        //Gain Mutex
        std::unique_lock<std::mutex> notify_lock(notify_mutex_);
        std::unique_lock<std::mutex> queue_lock(queue_mutex_);
        if(message_queue_.empty() && terminate_){
            break;
        }
    }
};

template <class T>
void InEventPort<T>::EnqueueMessage(T* t){
    //Log Receive Time
    //logger()->LogComponentEvent(*this, t, ModelLogger::ComponentEvent::RECEIVED);
    
    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(queue_mutex_);
    auto queue_size = message_queue_.size() + process_queue_size_;
    rx_count ++;


    //If we don't care about queueing, or we have room in our queue size, queue the message
    if(is_running() && (max_queue_size_ == -1 || max_queue_size_ > queue_size)){
        message_queue_.push(t);
        notify_lock_condition_.notify_all();
    }else{
        //logger()->LogComponentEvent(*this, t, ModelLogger::ComponentEvent::IGNORED);
        delete t;
    }
};



template <class T>
int InEventPort<T>::GetQueuedMessageCount(){
    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return message_queue_.size() + process_queue_size_;
};
                
#endif //INEVENTPORT_HPP