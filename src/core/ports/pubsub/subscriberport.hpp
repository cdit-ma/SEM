#ifndef BASE_PORT_SUBSCRIBER_HPP
#define BASE_PORT_SUBSCRIBER_HPP

#include <condition_variable>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <iostream>
#include "../../threadmanager.h"

#include "../translator.h"
#include "../port.h"
#include "../../modellogger.h"
#include "../../component.h"

//Interface for a standard templated SubscriberPort
template <class BaseType>
class SubscriberPort : public Port{
    public:
        SubscriberPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<void, BaseType>& callback_wrapper, const std::string& middleware);
        ~SubscriberPort();
        void SetMaxQueueSize(const int max_queue_size);

        using base_type = BaseType;
    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();

        void EnqueueMessage(BaseType* t);
        int GetQueuedMessageCount();
    private:
        bool rx(BaseType* t, bool process_message = true);
        void receive_loop();
    private:
        const CallbackWrapper<void, BaseType>& callback_wrapper_;

        
        //Queue Mutex responsible for these Variables
        std::mutex queue_mutex_;
        std::queue<BaseType*> message_queue_;
        int max_queue_size_ = -1;
        int processing_count_ = 0;

        const int terminate_timeout_ms_ = 10000;

        std::mutex notify_mutex_;
        bool terminate_ = false;
        std::condition_variable notify_lock_condition_;

        
        std::mutex rx_setup_mutex_;
        bool rx_setup_ = false;
        std::condition_variable rx_setup_condition_;

        std::mutex control_mutex_;
        ThreadManager* thread_manager_ = 0;
};

template <class BaseType>
SubscriberPort<BaseType>::SubscriberPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<void, BaseType>& callback_wrapper, const std::string& middleware)
:Port(component, port_name, Port::Kind::SUBSCRIBER, middleware),
callback_wrapper_(callback_wrapper)
{

};

template <class BaseType>
SubscriberPort<BaseType>::~SubscriberPort(){
    HandleTerminate();
};

template <class BaseType>
void SubscriberPort<BaseType>::SetMaxQueueSize(int max_queue_size){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    max_queue_size_ = max_queue_size;
};

template <class BaseType>
bool SubscriberPort<BaseType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(Port::HandlePassivate()){
        {
            //Wake up the threads sleep
            std::lock_guard<std::mutex> lock2(notify_mutex_);
            terminate_ = true;
            notify_lock_condition_.notify_all();
        }

        if(thread_manager_){
            return thread_manager_->Terminate();
        }
        return true;
    }
    return false;
};

template <class BaseType>
bool SubscriberPort<BaseType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(Port::HandleActivate()){
        if(thread_manager_){
            thread_manager_->Activate();
        }
        return true;
    }
    return false;
};



template <class BaseType>
bool SubscriberPort<BaseType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(Port::HandleConfigure()){
        if(!thread_manager_){
            thread_manager_ = new ThreadManager();
            auto future = std::async(std::launch::async, &SubscriberPort<BaseType>::receive_loop, this);
            thread_manager_->SetFuture(std::move(future));
            return thread_manager_->Configure();
        }else{
            std::cerr << "Have extra thread manager" << std::endl;
        }
    }
    return false;
};


template <class BaseType>
bool SubscriberPort<BaseType>::HandleTerminate(){
    SubscriberPort<BaseType>::HandlePassivate();
    std::unique_lock<std::mutex> lock(control_mutex_);
    if(Port::HandleTerminate()){

        if(thread_manager_){
            delete thread_manager_;
            thread_manager_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseType>
bool SubscriberPort<BaseType>::rx(BaseType* t, bool process_message){
    if(t){
        //Only process the message if we are running and we have a callback, and we aren't meant to ignore
        process_message &= is_running() && callback_wrapper_.callback_fn;

        if(process_message){
            //Call into the function and log
            logger()->LogComponentEvent(*this, *t, ModelLogger::ComponentEvent::STARTED_FUNC);
            callback_wrapper_.callback_fn(*t);
            logger()->LogComponentEvent(*this, *t, ModelLogger::ComponentEvent::FINISHED_FUNC);
        }

        EventProcessed(*t, process_message);
        delete t;
        return process_message;
    }
    return false;
};

template <class BaseType>
void SubscriberPort<BaseType>::receive_loop(){
    thread_manager_->Thread_Configured();

    //Store a queue of messages
    std::queue<BaseType*> queue;
    
    if(thread_manager_->Thread_WaitForActivate()){
        thread_manager_->Thread_Activated();
        bool running = true;
        while(running){
            {
                std::unique_lock<std::mutex> notify_lock(notify_mutex_);
                notify_lock_condition_.wait(notify_lock, [this]{
                    if(terminate_){
                        //Wake up if the termination flag has been set
                        return true;
                    }else{
                        //Wake up if we have new messages to process
                        std::unique_lock<std::mutex> queue_lock(queue_mutex_);
                        return message_queue_.size() > 0;
                    }
                });

                //Gain Mutex
                std::unique_lock<std::mutex> queue_lock(queue_mutex_);
                //Swap out the queue's, and release the mutex
                message_queue_.swap(queue);
                //Update the current processing count
                processing_count_ += queue.size();

                if(terminate_ && queue.empty()){
                    running = false;
                }
            }

            while(!queue.empty()){
                auto m = queue.front();
                queue.pop();

                //If the component is Passivated, this will return false instantaneously
                rx(m);
                
                std::unique_lock<std::mutex> queue_lock(queue_mutex_);
                //Decrement our count of how many messages are currently being processed
                processing_count_ --;
            }
        }
    }
    thread_manager_->Thread_Terminated();
};

template <class BaseType>
void SubscriberPort<BaseType>::EnqueueMessage(BaseType* t){
    if(t){
        //Log the recieving
        EventRecieved(*t);
        
        std::unique_lock<std::mutex> lock(queue_mutex_);
        //Sum the total number of messages we are processing
        auto queue_size = message_queue_.size() + processing_count_;

        //We should enqueue the message, if we are running, and we have room in our queue size (Or we don't care about queue size)
        bool enqueue_message = is_running() && (max_queue_size_ == -1 || max_queue_size_ > queue_size);

        if(enqueue_message){
            message_queue_.push(t);
            //Notify the thread that we have new messages
            notify_lock_condition_.notify_all();
        }else{
            lock.unlock();
            //Call the rx function, saying that we will ignore the message
            rx(t, false);
        }
    }
};

template <class BaseType>
int SubscriberPort<BaseType>::GetQueuedMessageCount(){
    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return message_queue_.size() + processing_count_;
};

#endif // BASE_PORT_SUBSCRIBE_HPP