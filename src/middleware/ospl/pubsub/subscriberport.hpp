#ifndef OSPL_PORT_SUBSCRIBER_HPP
#define OSPL_PORT_SUBSCRIBER_HPP

#include <middleware/ospl/helper.hpp>
#include <middleware/ospl/pubsub/datareaderlistener.hpp>
#include <core/ports/pubsub/subscriberport.hpp>

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>

namespace ospl{
    template <class BaseType, class OsplType>
    class SubscriberPort: public ::SubscriberPort<BaseType>{
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
        void recv_loop();

        ::Base::Translator<BaseType, OsplType> translator;

        //Define the Attributes this port uses
        std::shared_ptr<Attribute> subscriber_name_;
        std::shared_ptr<Attribute> domain_id_;
        std::shared_ptr<Attribute> topic_name_;
        //Not Yet implemented
        std::shared_ptr<Attribute> qos_path_;
        std::shared_ptr<Attribute> qos_name_;

        ThreadManager* thread_manager_ = 0;

        
        bool interupt_ = false;
        int data_available_count = 0;
        std::mutex control_mutex_;
        std::thread* recv_thread_ = 0;
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
        if(!thread_manager_){
            thread_manager_ = new ThreadManager();
            auto future = std::async(std::launch::async, &SubscriberPort<BaseType, OsplType>::recv_loop, this);
            thread_manager_->SetFuture(std::move(future));
            return thread_manager_->Configure();
        }else{
            std::cerr << "Have extra thread manager" << std::endl;
        }
    }
    return false;
};

template <class BaseType, class OsplType>
bool ospl::SubscriberPort<BaseType, OsplType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandlePassivate()){ 
        if(thread_manager_){
            return thread_manager_->Terminate();
        }
        return true;
    }
    return false;
};

template <class BaseType, class OsplType>
bool ospl::SubscriberPort<BaseType, OsplType>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::SubscriberPort<BaseType>::HandleTerminate()){
        if(thread_manager_){
            delete thread_manager_;
            thread_manager_ = 0;
        }
        return true;
    }
    return false;
};


template <class BaseType, class OsplType>
void ospl::SubscriberPort<BaseType, OsplType>::recv_loop(){
    dds::sub::DataReader<OsplType> reader_ = dds::sub::DataReader<OsplType>(dds::core::null);
    ospl::DataReaderListener<BaseType, OsplType> listener(this);

    bool success = true;
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
        success = false;
    }

    if(success){
        thread_manager_->Thread_Configured();
        if(thread_manager_->Thread_WaitForActivate()){
            thread_manager_->Thread_Activated();
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
        }
        //Blocks for the DataReaderListener to finish
        reader_.close();
    }
    thread_manager_->Thread_Terminated();
};

#endif //OSPL_PORT_SUBSCRIBER_HPP