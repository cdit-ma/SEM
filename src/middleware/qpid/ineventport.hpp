#ifndef QPID_INEVENTPORT_H
#define QPID_INEVENTPORT_H

#include <core/eventports/prototranslator.h>
#include <core/eventports/ineventport.hpp>

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
    template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function);
            ~InEventPort(){
                Activatable::Terminate();
            }

            protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            void recv_loop();
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

template <class T, class S>
qpid::InEventPort<T, S>::InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function):
::InEventPort<T>(component, name, callback_function, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class T, class S>
bool qpid::InEventPort<T, S>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = topic_name_->String().length() >= 0 && broker_->String().length() >= 0;
    if(valid && ::InEventPort<T>::HandleConfigure()){
        if(!recv_thread_){
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            recv_thread_ = new std::thread(&qpid::InEventPort<T, S>::recv_loop, this);
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
};

template <class T, class S>
bool qpid::InEventPort<T, S>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandlePassivate()){ 
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

template <class T, class S>
bool qpid::InEventPort<T, S>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandleTerminate()){
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

template <class T, class S>
void qpid::InEventPort<T, S>::recv_loop(){
    auto state = ThreadState::STARTED;
    try{
        std::lock_guard<std::mutex> lock(connection_mutex_);
        //Construct a Qpid Connection
        connection_ = qpid::messaging::Connection(broker_->String());
        //Open the connection
        connection_.open();
        auto session = connection_.createSession();
        receiver_ = session.createReceiver( "amq.topic/"  + topic_name_->String());
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to startup QPID AMQP Reciever") + ex.what());
        state = ThreadState::ERROR_;
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
        EventPort::LogActivation();
        bool run = true;
        while(run){
            try{
                auto sample = receiver_.fetch();
                if(receiver_.isClosed()){
                    run = false;
                }
                std::string str = sample.getContent();
                auto m = proto::decode<S>(str);
                this->EnqueueMessage(m);
            }
            catch(const std::exception& ex){
                //Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to fetch QPID Messages") + ex.what());
                run = false;
            }
        }
        EventPort::LogPassivation();
    }
};

#endif //QPID_INEVENTPORT_H
