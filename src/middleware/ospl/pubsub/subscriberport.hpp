#ifndef OSPL_PORTSUBSCRIBER_HPP
#define OSPL_PORTSUBSCRIBER_HPP

#include <middleware/ospl/helper.hpp>
#include <middleware/ospl/pubsub/datareaderlistener.hpp>
#include <core/ports/pubsub/subscriberport.hpp>

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>


namespace ospl{
    template <class BaseType, class OsplType> class SubscriberPort: public ::SubscriberPort<BaseType>{
        friend class DataReaderListener<BaseType, OsplType>;
    public:
        SubscriberPort(std::weak_ptr<Component> component, std::string name, const CallbackWrapper<void, BaseType>& callback_wrapper);
        ~SubscriberPort(){
            Activatable::Terminate();
        };
        void notify();
    protected:
        bool HandleConfigure();
        bool HandlePassivate();
        bool HandleTerminate();
    private:
        void receive_loop();

        ::Base::Translator<BaseType, OsplType> translator;

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
        std::mutex control_mutex_;
        std::thread* rec_thread_ = 0;
        std::mutex notify_mutex_;
        std::condition_variable notify_lock_condition_;
   };
};

template <class BaseType, class OsplType>
ospl::SubscriberPort<BaseType, OsplType>::SubscriberPort(std::weak_ptr<Component> component, std::string name, const CallbackWrapper<void, BaseType>& callback_wrapper):
::SubscriberPort<BaseType>(component, name, callback_wrapper, "ospl"){
    subscriber_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "subscriber_name").lock();
    domain_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "domain_id").lock();
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    qos_path_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_path").lock();
    qos_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_name").lock();

    //Set default subscriber
    subscriber_name_->set_String("In_" + this->get_name());
};

template <class BaseType, class OsplType>
bool ospl::SubscriberPort<BaseType, OsplType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = topic_name_->String().length() >= 0;

    if(valid && ::SubscriberPort<BaseType>::HandleConfigure()){
        if(!rec_thread_){
            {
                std::unique_lock<std::mutex> lock(notify_mutex_);
                interupt_ = false;
            }
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            rec_thread_ = new std::thread(&ospl::SubscriberPort<BaseType, OsplType>::receive_loop, this);
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
};

template <class BaseType, class OsplType>
bool ospl::SubscriberPort<BaseType, OsplType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandlePassivate()){ 
        //Set the terminate state in the passivation
        std::lock_guard<std::mutex> lock(notify_mutex_);
        interupt_ = true;
        notify();
        return true;
    }
    return false;
};

template <class BaseType, class OsplType>
bool ospl::SubscriberPort<BaseType, OsplType>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandleTerminate()){
        if(rec_thread_){
            //Join our ospl_thread
            rec_thread_->join();
            delete rec_thread_;
            rec_thread_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseType, class OsplType>
void ospl::SubscriberPort<BaseType, OsplType>::notify(){
   //Called by the DataReaderListener to notify our SubscriberPort thread to get new data
   notify_lock_condition_.notify_all();
};

template <class BaseType, class OsplType>
void ospl::SubscriberPort<BaseType, OsplType>::receive_loop(){
    dds::sub::DataReader<OsplType> reader_ = dds::sub::DataReader<OsplType>(dds::core::null);
    ospl::DataReaderListener<BaseType, OsplType> listener(this);

    auto state = ThreadState::STARTED;

    try{
       //Construct a DDS Paosplcipant, Subscriber, Topic and Reader
       auto helper = DdsHelper::get_dds_helper();    
       auto participant = helper->get_participant(domain_id_->Integer());
       auto topic = get_topic<OsplType>(participant, topic_name_->String());

       auto subscriber = helper->get_subscriber(participant, subscriber_name_->String());
       reader_ = get_data_reader<OsplType>(subscriber, topic, qos_path_->String(), qos_name_->String());

       //Attach listener to only respond to data_available()
       reader_.listener(&listener, dds::core::status::StatusMask::data_available());
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to startup OSPL DDS Reciever") + ex.what());
        state = ThreadState::TERMINATED;
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
        Port::LogActivation();
        
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
        Port::LogPassivation();
        //Blocks for the DataReaderListener to finish
        reader_.close();
    }
};

#endif //OSPL_PORTSUBSCRIBER_HPP