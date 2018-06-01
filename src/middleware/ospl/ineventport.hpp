#ifndef OSPL_INEVENTPORT_H
#define OSPL_INEVENTPORT_H

#include <middleware/ospl/helper.hpp>
#include <middleware/ospl/datareaderlistener.hpp>
#include <core/eventports/ineventport.hpp>

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>


namespace ospl{
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
        void receive_loop();

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
        std::mutex control_mutex_;
        std::thread* rec_thread_ = 0;
        std::mutex notify_mutex_;
        std::condition_variable notify_lock_condition_;
   };
};

template <class T, class S>
ospl::InEventPort<T, S>::InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function):
::InEventPort<T>(component, name, callback_function, "ospl"){
    subscriber_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "subscriber_name").lock();
    domain_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "domain_id").lock();
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    qos_path_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_path").lock();
    qos_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_name").lock();

    //Set default subscriber
    subscriber_name_->set_String("In_" + this->get_name());
};

template <class T, class S>
bool ospl::InEventPort<T, S>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = topic_name_->String().length() >= 0;

    if(valid && ::InEventPort<T>::HandleConfigure()){
        if(!rec_thread_){
            {
                std::unique_lock<std::mutex> lock(notify_mutex_);
                interupt_ = false;
            }
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            rec_thread_ = new std::thread(&ospl::InEventPort<T, S>::receive_loop, this);
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
};

template <class T, class S>
bool ospl::InEventPort<T, S>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandlePassivate()){ 
        //Set the terminate state in the passivation
        std::lock_guard<std::mutex> lock(notify_mutex_);
        interupt_ = true;
        notify();
        return true;
    }
    return false;
};

template <class T, class S>
bool ospl::InEventPort<T, S>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandleTerminate()){
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

template <class T, class S>
void ospl::InEventPort<T, S>::notify(){
   //Called by the DataReaderListener to notify our InEventPort thread to get new data
   notify_lock_condition_.notify_all();
};

template <class T, class S>
void ospl::InEventPort<T, S>::receive_loop(){
    dds::sub::DataReader<S> reader_ = dds::sub::DataReader<S>(dds::core::null);
    ospl::DataReaderListener<T, S> listener(this);

    auto state = ThreadState::STARTED;

    try{
       //Construct a DDS Paosplcipant, Subscriber, Topic and Reader
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

#endif //OSPL_INEVENTPORT_H