#ifndef RTI_OUTEVENTPORT_H
#define RTI_OUTEVENTPORT_H

#include <middleware/rti/helper.hpp>
#include <core/eventports/outeventport.hpp>

#include <string>
#include <mutex>
#include <exception>

namespace rti{
    template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
       public:
            OutEventPort(std::weak_ptr<Component> component, std::string name);
            ~OutEventPort(){
                Activatable::Terminate();
            }
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        public:
            bool tx(const T& message);
        private:
            bool setup_tx();
            int count = 0;

            ::Base::Translator<T,S> translator;

             //Define the Attributes this port uses
            std::shared_ptr<Attribute> publisher_name_;
            std::shared_ptr<Attribute> domain_id_;
            std::shared_ptr<Attribute> topic_name_;
            //Not Yet implemented
            std::shared_ptr<Attribute> qos_path_;
            std::shared_ptr<Attribute> qos_name_;

            std::mutex control_mutex_;
            dds::pub::DataWriter<S> writer_ = dds::pub::DataWriter<S>(dds::core::null);
   }; 
};


template <class T, class S>
rti::OutEventPort<T, S>::OutEventPort(std::weak_ptr<Component> component, std::string name): ::OutEventPort<T>(component, name, "rti"){
    publisher_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "publisher_name").lock();
    domain_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "domain_id").lock();
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    qos_path_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_path").lock();
    qos_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_name").lock();

    //Set default publisher_name
    publisher_name_->set_String("In_" + this->get_name());
};


template <class T, class S>
bool rti::OutEventPort<T, S>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool valid = topic_name_->String().length() > 0;

    if(valid && ::OutEventPort<T>::HandleConfigure()){

        return setup_tx();
    }
    return false;
};

template <class T, class S>
bool rti::OutEventPort<T, S>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::HandlePassivate()){
        if(writer_ != dds::core::null){
            writer_.close();
            writer_ = dds::pub::DataWriter<S>(dds::core::null);
        }
        return true;
    }
    return false;
};

template <class T, class S>
bool rti::OutEventPort<T, S>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    return ::OutEventPort<T>::HandleTerminate();
};


template <class T, class S>
bool rti::OutEventPort<T, S>::tx(const T& message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool should_send = ::OutEventPort<T>::tx(message);

    if(should_send){
        if(writer_ != dds::core::null){
            auto m = translator.BaseToMiddleware(message);
            if(m){
                //De-reference the message and send
                writer_.write(*m);
                delete m;
                return true;
            }
        }else{
            Log(Severity::DEBUG).Context(this).Func(GET_FUNC).Msg("Writer unexpectedly null");
        }
    }
    return false;
};

template <class T, class S>
bool rti::OutEventPort<T, S>::setup_tx(){
    if(writer_ == dds::core::null){
        //Construct a DDS Participant, Publisher, Topic and Writer
        auto helper = DdsHelper::get_dds_helper();   
        auto participant = helper->get_participant(domain_id_->Integer());
        auto topic = get_topic<S>(participant, topic_name_->String());
        auto publisher = helper->get_publisher(participant, publisher_name_->String());
        writer_ = get_data_writer<S>(publisher, topic, qos_path_->String(), qos_name_->String());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return true;
    }
    return false;
};


#endif //RTI_OUTEVENTPORT_H