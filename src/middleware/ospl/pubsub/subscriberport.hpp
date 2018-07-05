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
        ~SubscriberPort(){this->Terminate();};
    protected:
        void HandleActivate();
        void HandleConfigure();
        void HandlePassivate();
        void HandleTerminate();
    private:
        void InterruptLoop();
        void Loop(ThreadManager& thread_manager, dds::sub::DataReader<OsplType> reader);
        dds::sub::DataReader<OsplType> GetReader();

        ::Base::Translator<BaseType, OsplType> translator;

        //Define the Attributes this port uses
        std::shared_ptr<Attribute> subscriber_name_;
        std::shared_ptr<Attribute> domain_id_;
        std::shared_ptr<Attribute> topic_name_;
        //Not Yet implemented
        std::shared_ptr<Attribute> qos_path_;
        std::shared_ptr<Attribute> qos_name_;

        std::mutex mutex_;
        std::unique_ptr<ThreadManager> thread_manager_;
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
void ospl::SubscriberPort<BaseType, OsplType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!thread_manager_)
        throw std::runtime_error("ospl Subscriber Port: '" + this->get_name() + "': Has no ThreadManager!");

    thread_manager_->Activate();
    //Wait for our recv thjread to be up
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ::SubscriberPort<BaseType>::HandleActivate();
};

template <class BaseType, class OsplType>
void ospl::SubscriberPort<BaseType, OsplType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_)
        throw std::runtime_error("ospl Subscriber Port: '" + this->get_name() + "': Has an errant ThreadManager!");

    thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
    auto reader = GetReader();
    auto future = std::async(std::launch::async, &ospl::SubscriberPort<BaseType, OsplType>::Loop, this, std::ref(*thread_manager_), std::move(reader));
    thread_manager_->SetFuture(std::move(future));
    if(!thread_manager_->Configure()){
        throw std::runtime_error("ospl Subscriber Port: '" + this->get_name() + "': setup failed!");
    }
    ::SubscriberPort<BaseType>::HandleConfigure();
};

template <class BaseType, class OsplType>
void ospl::SubscriberPort<BaseType, OsplType>::HandlePassivate(){
    InterruptLoop();
    ::SubscriberPort<BaseType>::HandlePassivate();
};


template <class BaseType, class OsplType>
void ospl::SubscriberPort<BaseType, OsplType>::InterruptLoop(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_){
        thread_manager_->SetTerminate();
    }
};


template <class BaseType, class OsplType>
void ospl::SubscriberPort<BaseType, OsplType>::HandleTerminate(){
    InterruptLoop();

    std::lock_guard<std::mutex> lock(mutex_);
    thread_manager_.reset();
    
    ::SubscriberPort<BaseType>::HandleTerminate();
};

template <class BaseType, class OsplType>
dds::sub::DataReader<OsplType> ospl::SubscriberPort<BaseType, OsplType>::GetReader(){
    //Construct a DDS Participant, Subscriber, Topic and Reader
    auto& helper = get_dds_helper();
    auto participant = helper.get_participant(domain_id_->Integer());
    auto topic = get_topic<OsplType>(participant, topic_name_->String());

    auto subscriber = helper.get_subscriber(participant, subscriber_name_->String());
    return get_data_reader<OsplType>(subscriber, topic, qos_path_->String(), qos_name_->String());
};


template <class BaseType, class OsplType>
void ospl::SubscriberPort<BaseType, OsplType>::Loop(ThreadManager& thread_manager, dds::sub::DataReader<OsplType> reader){
    ospl::DataReaderListener<BaseType, OsplType> listener(this);
    //Attach the listener
    reader.listener(&listener, dds::core::status::StatusMask::data_available());

    thread_manager.Thread_Configured();

    if(thread_manager.Thread_WaitForActivate()){
        thread_manager.Thread_Activated();
        thread_manager.Thread_WaitForTerminate();
    }
    //Blocks for the DataReaderListener to finish
    reader.close();
    thread_manager.Thread_Terminated();
};

#endif //OSPL_PORT_SUBSCRIBER_HPP