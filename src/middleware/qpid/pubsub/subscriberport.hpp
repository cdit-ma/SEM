#ifndef QPID_PORT_SUBSCRIBER_HPP
#define QPID_PORT_SUBSCRIBER_HPP

#include <middleware/proto/prototranslator.h>
#include <core/ports/pubsub/subscriberport.hpp>

#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <functional>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Duration.h>


namespace qpid{
    template <class BaseType, class ProtoType> class SubscriberPort: public ::SubscriberPort<BaseType>{
        public:
            SubscriberPort(std::weak_ptr<Component> component, std::string name, std::function<void (BaseType&) > callback_function);
            ~SubscriberPort(){
                Activatable::Terminate();
            }

            protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            void recv_loop();

            ::Proto::Translator<BaseType, ProtoType> translator;

            std::mutex connection_mutex_;
            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Receiver receiver_ = 0;

            std::mutex control_mutex_;
            std::thread* recv_thread_ = 0;

            std::mutex thread_state_mutex_;
            ThreadState thread_state_;
            std::condition_variable thread_state_condition_;

            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;
    };
};

template <class BaseType, class ProtoType>
qpid::SubscriberPort<BaseType, ProtoType>::SubscriberPort(std::weak_ptr<Component> component, std::string name, std::function<void (BaseType&) > callback_function):
::SubscriberPort<BaseType>(component, name, callback_function, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseType, class ProtoType>
bool qpid::SubscriberPort<BaseType, ProtoType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = topic_name_->String().length() >= 0 && broker_->String().length() >= 0;
    if(valid && ::SubscriberPort<BaseType>::HandleConfigure()){
        if(!recv_thread_){
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            recv_thread_ = new std::thread(&qpid::SubscriberPort<BaseType, ProtoType>::recv_loop, this);
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
};

template <class BaseType, class ProtoType>
bool qpid::SubscriberPort<BaseType, ProtoType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandlePassivate()){ 
        //Set the terminate state in the passivation
        std::lock_guard<std::mutex> lock(connection_mutex_);
        if(connection_.isOpen()){
            //do passivation things here
            receiver_.close();
        }
        return true;
    }
    return false;
};

template <class BaseType, class ProtoType>
bool qpid::SubscriberPort<BaseType, ProtoType>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandleTerminate()){
        if(recv_thread_){
            //Join our rti_thread
            recv_thread_->join();
            delete recv_thread_;
            recv_thread_ = 0;

            connection_.close();
            connection_ = 0;
            receiver_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseType, class ProtoType>
void qpid::SubscriberPort<BaseType, ProtoType>::recv_loop(){
    auto state = ThreadState::STARTED;
    try{
        std::lock_guard<std::mutex> lock(connection_mutex_);
        //Construct a Qpid Connection
        connection_ = qpid::messaging::Connection(broker_->String());
        //Open the connection
        connection_.open();
        auto session = connection_.createSession();
        receiver_ = session.createReceiver( "amq.topic/pubsub/"  + topic_name_->String());
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to startup QPID AMQP Reciever") + ex.what());
        state = ThreadState::TERMINATED;
    }

    //Change the state to be Configured
    {
        std::lock_guard<std::mutex> lock(thread_state_mutex_);
        thread_state_ = state;
    }

    //Sleep for 250 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    thread_state_condition_.notify_all();

    if(state == ThreadState::STARTED && Activatable::BlockUntilStateChanged(Activatable::State::RUNNING)){
        //Log the port becoming online
        Port::LogActivation();
        bool run = true;
        while(run){
            try{
                auto sample = receiver_.fetch();
                if(receiver_.isClosed()){
                    run = false;
                }
                std::string str = sample.getContent();
                auto m = translator.StringToBase(str);
                this->EnqueueMessage(m);
            }
            catch(const std::exception& ex){
                //Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to fetch QPID Messages") + ex.what());
                run = false;
            }
        }
        Port::LogPassivation();
    }
};

#endif //QPID_PORT_SUBSCRIBER_HPP
