#ifndef OSPL_PORT_PUBLISHER_HPP
#define OSPL_PORT_PUBLISHER_HPP

#include <middleware/ospl/helper.hpp>
#include <core/ports/pubsub/publisherport.hpp>


#include <string>
#include <mutex>
#include <exception>

namespace ospl{
    template <class BaseType, class OsplType> class PublisherPort: public ::PublisherPort<BaseType>{
       public:
            PublisherPort(std::weak_ptr<Component> component, std::string name);
            ~PublisherPort(){this->Terminate();};
            void Send(const BaseType& message);
        protected:
            void HandleConfigure();
            void HandlePassivate();
            void HandleTerminate();
        private:
            void SetupWriter();
            void DestructWriter();
            
            ::Base::Translator<BaseType, OsplType> translator;

             //Define the Attributes this port uses
            std::shared_ptr<Attribute> publisher_name_;
            std::shared_ptr<Attribute> domain_id_;
            std::shared_ptr<Attribute> topic_name_;
            //Not Yet implemented
            std::shared_ptr<Attribute> qos_path_;
            std::shared_ptr<Attribute> qos_name_;

            std::mutex writer_mutex_;
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
void ospl::PublisherPort<BaseType, OsplType>::HandleConfigure(){
    SetupWriter();
    ::PublisherPort<BaseType>::HandleConfigure();
};


template <class BaseType, class OsplType>
void ospl::PublisherPort<BaseType, OsplType>::HandlePassivate(){
    DestructWriter();
    ::PublisherPort<BaseType>::HandlePassivate();
};


template <class BaseType, class OsplType>
void ospl::PublisherPort<BaseType, OsplType>::HandleTerminate(){
    DestructWriter();
    ::PublisherPort<BaseType>::HandleTerminate();
};


template <class BaseType, class OsplType>
void ospl::PublisherPort<BaseType, OsplType>::Send(const BaseType& message){
    //Log the recieving
    this->EventRecieved(message);

    if(this->is_running()){
        //std::lock_guard<std::mutex> lock(writer_mutex_);
        if(writer_ != dds::core::null){
            auto m = translator.BaseToMiddleware(message);
            if(m){
                //De-reference the message and send
                writer_.write(*m);
                delete m;
                this->EventProcessed(message);
                this->logger().LogComponentEvent(*this, message, ModelLogger::ComponentEvent::SENT);
            }
        }
    }
    this->EventIgnored(message);
};

template <class BaseType, class OsplType>
void ospl::PublisherPort<BaseType, OsplType>::SetupWriter(){
    std::lock_guard<std::mutex> lock(writer_mutex_);
    if(writer_ == dds::core::null){
        //Construct a DDS Participant, Publisher, Topic and Writer
        auto& helper = get_dds_helper();   
        auto participant = helper.get_participant(domain_id_->Integer());
        auto topic = get_topic<OsplType>(participant, topic_name_->String());
        auto publisher = helper.get_publisher(participant, publisher_name_->String());
        writer_ = get_data_writer<OsplType>(publisher, topic, qos_path_->String(), qos_name_->String());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }else{
        throw std::runtime_error("ospl Publisher Port: '" + this->get_name() + "': Has an errant writer!");
    }
};

template <class BaseType, class OsplType>
void ospl::PublisherPort<BaseType, OsplType>::DestructWriter(){
    std::lock_guard<std::mutex> lock(writer_mutex_);
    if(writer_ != dds::core::null){
        writer_.close();
        writer_ = dds::pub::DataWriter<OsplType>(dds::core::null);
    }
};


#endif //OSPL_PORT_PUBLISHER_HPP