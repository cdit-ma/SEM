#ifndef OSPL_POR_PUBLISHER_HPP
#define OSPL_POR_PUBLISHER_HPP

#include <middleware/ospl/helper.hpp>
#include <core/ports/pubsub/publisherport.hpp>

#include <string>
#include <mutex>
#include <exception>

namespace ospl{
    template <class BaseType, class OsplType> class PublisherPort: public ::PublisherPort<BaseType>{
       public:
            PublisherPort(std::weak_ptr<Component> component, std::string name);
           ~PublisherPort(){
                Activatable::Terminate();
            }
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        public:
            bool Send(const BaseType& message);
        private:
            bool setup_tx();

            ::Base::Translator<BaseType, OsplType> translator;

             //Define the Attributes this port uses
            std::shared_ptr<Attribute> publisher_name_;
            std::shared_ptr<Attribute> domain_id_;
            std::shared_ptr<Attribute> topic_name_;
            //Not Yet implemented
            std::shared_ptr<Attribute> qos_path_;
            std::shared_ptr<Attribute> qos_name_;

            std::mutex control_mutex_;
            dds::pub::DataWriter<OsplType> writer_ = dds::pub::DataWriter<OsplType>(dds::core::null);
   }; 
};


template <class BaseType, class OsplType>
ospl::PublisherPort<BaseType, OsplType>::PublisherPort(std::weak_ptr<Component> component, std::string name): ::PublisherPort<BaseType>(component, name, "ospl"){
    publisher_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "publisher_name").lock();
    domain_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "domain_id").lock();
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    qos_path_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_path").lock();
    qos_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_name").lock();

    //Set default publisher_name
    publisher_name_->set_String("In_" + this->get_name());
};

template <class BaseType, class OsplType>
bool ospl::PublisherPort<BaseType, OsplType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool valid = topic_name_->String().length() > 0;

    if(valid && ::PublisherPort<BaseType>::HandleConfigure()){

        return setup_tx();
    }
    return false;
};

template <class BaseType, class OsplType>
bool ospl::PublisherPort<BaseType, OsplType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::PublisherPort<BaseType>::HandlePassivate()){
        if(writer_ != dds::core::null){
            writer_.close();
            writer_ = dds::pub::DataWriter<OsplType>(dds::core::null);
        }
        return true;
    }
    return false;
};

template <class BaseType, class OsplType>
bool ospl::PublisherPort<BaseType, OsplType>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    return ::PublisherPort<BaseType>::HandleTerminate();
};

template <class BaseType, class OsplType>
bool ospl::PublisherPort<BaseType, OsplType>::Send(const BaseType& message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool should_send = ::PublisherPort<BaseType>::Send(message);

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

template <class BaseType, class OsplType>
bool ospl::PublisherPort<BaseType, OsplType>::setup_tx(){
    if(writer_ == dds::core::null){
        //Construct a DDS Paosplcipant, Publisher, Topic and Writer
        auto helper = DdsHelper::get_dds_helper();   
        auto participant = helper->get_participant(domain_id_->Integer());
        auto topic = get_topic<OsplType>(participant, topic_name_->String());
        auto publisher = helper->get_publisher(participant, publisher_name_->String());
        writer_ = get_data_writer<OsplType>(publisher, topic, qos_path_->String(), qos_name_->String());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return true;
    }
    return false;
};


#endif //OSPL_POR_PUBLISHER_HPP