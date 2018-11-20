#ifndef BASE_PORT_SUBSCRIBER_HPP
#define BASE_PORT_SUBSCRIBER_HPP

#include <condition_variable>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <iostream>
#include "../../threadmanager.h"

#include "../port.h"
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
        virtual void HandleConfigure();
        virtual void HandleActivate();
        virtual void HandlePassivate();
        virtual void HandleTerminate();

        void EnqueueMessage(std::unique_ptr<BaseType> message);
        int GetQueuedMessageCount();
    private:
        void InterruptLoop();
        void ProcessLoop();
        void rx(BaseType& t, bool process_message = true);
    private:
        const CallbackWrapper<void, BaseType>& callback_wrapper_;

        
        //Queue Mutex responsible for these Variables
        std::mutex queue_mutex_;
        std::condition_variable queue_condition_;
        std::queue< std::unique_ptr<BaseType> > message_queue_;
        int max_queue_size_ = -1;
        int processing_count_ = 0;
        bool terminate_ = false;

        std::mutex thread_manager_mutex_;
        std::unique_ptr<ThreadManager> thread_manager_;
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
void SubscriberPort<BaseType>::InterruptLoop(){
    //Wake up the threads sleep
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        terminate_ = true;
    }
    queue_condition_.notify_all();
}


template <class BaseType>
void SubscriberPort<BaseType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(!thread_manager_){
        thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
        auto future = std::async(std::launch::async, &SubscriberPort<BaseType>::ProcessLoop, this);
        thread_manager_->SetFuture(std::move(future));
        thread_manager_->WaitForConfigured();
    }else{
        throw std::runtime_error("SubscriberPort has an active ThreadManager");
    }
    Port::HandleConfigure();
};

template <class BaseType>
void SubscriberPort<BaseType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_){
        thread_manager_->Activate();
    }else{
        throw std::runtime_error("Got no Thread Manager");
    }
    Port::HandleActivate();
};


template <class BaseType>
void SubscriberPort<BaseType>::HandlePassivate(){
    InterruptLoop();
    Port::HandlePassivate();
};

template <class BaseType>
void SubscriberPort<BaseType>::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(thread_manager_mutex_);
    if(thread_manager_){
        thread_manager_->Terminate();
        thread_manager_.reset();
    }
    Port::HandleTerminate();
};

template <class BaseType>
void SubscriberPort<BaseType>::rx(BaseType& message, bool process_message){
    //Only process the message if we are running and we have a callback, and we aren't meant to ignore
    if(process_message && process_event() && callback_wrapper_.callback_fn){
        try{
            logger().LogPortUtilizationEvent(*this, message, Logger::UtilizationEvent::STARTED_FUNC);
            callback_wrapper_.callback_fn(message);
            logger().LogPortUtilizationEvent(*this, message, Logger::UtilizationEvent::FINISHED_FUNC);
        }catch(const std::exception& ex){
            CallbackException exception(ex.what());
            logger().LogPortUtilizationEvent(*this, message, Logger::UtilizationEvent::EXCEPTION, exception.what());
        }
        EventProcessed(message);
    }else{
        EventIgnored(message);
    }
};

template <class BaseType>
void SubscriberPort<BaseType>::ProcessLoop(){
    //Store a queue of messages
    std::queue< std::unique_ptr<BaseType> > queue;

    //Notify that the thread is configured
    thread_manager_->Thread_Configured();

    if(thread_manager_->Thread_WaitForActivate()){
        thread_manager_->Thread_Activated();

        bool running = true;
        while(running){
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                queue_condition_.wait(lock, [this]{
                    //Wake up if the termination flag has been set
                    return terminate_ || !message_queue_.empty();
                });
                
                //Swap out the queue's, and release the mutex
                message_queue_.swap(queue);
                //Update the current processing count
                processing_count_ += queue.size();

                if(terminate_){
                    running = false;
                    terminate_ = false;
                }
            }

            
            while(!queue.empty()){
                auto message = std::move(queue.front());
                queue.pop();

                //If the component is Passivated, this will return false instantaneously
                rx(*message);

                std::lock_guard<std::mutex> lock(queue_mutex_);
                //Decrement our count of how many messages are currently being processed
                processing_count_ --;
            }
        }
    }
    thread_manager_->Thread_Terminated();
};

template <class BaseType>
void SubscriberPort<BaseType>::EnqueueMessage(std::unique_ptr<BaseType> message){
    if(message){
        //Log that we've recieved
        EventRecieved(*message);
        
        std::lock_guard<std::mutex> lock(queue_mutex_);
        //Sum the total number of messages we are processing
        int queue_size = message_queue_.size() + processing_count_;

        //We should enqueue the message, if we are running, and we have room in our queue size (Or we don't care about queue size)
        bool enqueue_message = process_event() && (max_queue_size_ == -1 || max_queue_size_ > queue_size);

        if(enqueue_message){
            message_queue_.push(std::move(message));
            //Notify the thread that we have new messages
            queue_condition_.notify_all();
        }else{
            //Call the rx function, saying that we will ignore the message
            rx(*message, false);
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