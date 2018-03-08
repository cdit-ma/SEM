#ifndef RTI_INEVENTPORT_H
#define RTI_INEVENTPORT_H

#include <middleware/rti/helper.hpp>
#include <middleware/rti/datareaderlistener.hpp>
#include <core/eventports/ineventport.hpp>

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>

namespace rti{
    template <class T, class S> class InEventPort: public ::InEventPort<T>{
        friend class DataReaderListener<T,S>;
    public:
        InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function);
        ~InEventPort(){
            Activatable::Terminate();
        };
        void notify();
    protected:
        bool HandleConfigure();
        bool HandlePassivate();
        bool HandleTerminate();
    private:
        void recv_loop();

        ::Base::Translator<T,S> translator;

        //Define the Attributes this port uses
        std::shared_ptr<Attribute> subscriber_name_;
        std::shared_ptr<Attribute> domain_id_;
        std::shared_ptr<Attribute> topic_name_;
        //Not Yet implemented
        std::shared_ptr<Attribute> qos_path_;
        std::shared_ptr<Attribute> qos_name_;

        std::mutex thread_state_mutex_;
        ThreadState thread_state_;
        std::condition_variable thread_state_condition_;

        
        bool interupt_ = false;
        int data_available_count = 0;
        std::mutex control_mutex_;
        std::thread* recv_thread_ = 0;
        std::mutex notify_mutex_;
        std::condition_variable notify_lock_condition_;
   };
};

template <class T, class S>
rti::InEventPort<T, S>::InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function):
::InEventPort<T>(component, name, callback_function, "rti"){
    subscriber_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "subscriber_name").lock();
    domain_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "domain_id").lock();
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    qos_path_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_path").lock();
    qos_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_name").lock();

    //Set default subscriber
    subscriber_name_->set_String("In_" + this->get_name());
};

template <class T, class S>
bool rti::InEventPort<T, S>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = topic_name_->String().length() >= 0;
    if(valid && ::InEventPort<T>::HandleConfigure()){
        if(!recv_thread_){
            {
                std::unique_lock<std::mutex> lock(notify_mutex_);
                interupt_ = false;
            }
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            recv_thread_ = new std::thread(&rti::InEventPort<T, S>::recv_loop, this);
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
};

template <class T, class S>
bool rti::InEventPort<T, S>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandlePassivate()){ 
        //Set the terminate state in the passivation
        {
            std::lock_guard<std::mutex> lock(notify_mutex_);
            interupt_ = true;
        }
        notify();
        return true;
    }
    return false;
};

template <class T, class S>
bool rti::InEventPort<T, S>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandleTerminate()){
        if(recv_thread_){
            //Join our rti_thread
            recv_thread_->join();
            delete recv_thread_;
            recv_thread_ = 0;
        }
        return true;
    }
    return false;
};

template <class T, class S>
void rti::InEventPort<T, S>::notify(){
    notify_lock_condition_.notify_all();
};

template <class T, class S>
void rti::InEventPort<T, S>::recv_loop(){
    dds::sub::DataReader<S> reader_ = dds::sub::DataReader<S>(dds::core::null);
    rti::DataReaderListener<T, S> listener(this);

    auto state = ThreadState::STARTED;

    try{
       //Construct a DDS Participant, Subscriber, Topic and Reader
       auto helper = DdsHelper::get_dds_helper();    
       auto participant = helper->get_participant(domain_id_->Integer());
       auto topic = get_topic<S>(participant, topic_name_->String());

       auto subscriber = helper->get_subscriber(participant, subscriber_name_->String());
       reader_ = get_data_reader<S>(subscriber, topic, qos_path_->String(), qos_name_->String());
       
       //Attach listener to only respond to data_available()
       reader_.listener(&listener, dds::core::status::StatusMask::data_available());
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to startup OSPL DDS Reciever") + ex.what());
        state = ThreadState::ERROR_;
    }

    //Change the state to be Configured
    {
        std::lock_guard<std::mutex> lock(thread_state_mutex_);
        thread_state_ = state;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    thread_state_condition_.notify_all();

    if(state == ThreadState::STARTED && Activatable::BlockUntilStateChanged(Activatable::State::RUNNING)){
        //Log the port becoming online
        EventPort::LogActivation();
        
        while(true){ 
            {
                //Wait for next message
                std::unique_lock<std::mutex> lock(notify_mutex_);
                //Check to see if we've been interupted before sleeping the first time
                if(interupt_){
                    break;
                }else{
                    notify_lock_condition_.wait(lock);
                }
            }
        }
        EventPort::LogPassivation();
    }
    //Blocks for the DataReaderListener to finish
    reader_.close();
};

#endif //RTI_INEVENTPORT_H