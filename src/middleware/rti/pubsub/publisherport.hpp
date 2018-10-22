#ifndef RTI_PORT_PUBLISHER_HPP
#define RTI_PORT_PUBLISHER_HPP

#include <middleware/rti/helper.hpp>
#include <core/ports/pubsub/publisherport.hpp>


#include <string>
#include <mutex>
#include <exception>

namespace rti{
    template <class BaseType, class RtiType> class PublisherPort: public ::PublisherPort<BaseType>{
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
            
            ::Base::Translator<BaseType, RtiType> translator;

             //Define the Attributes this port uses
            std::shared_ptr<Attribute> publisher_name_;
            std::shared_ptr<Attribute> domain_id_;
            std::shared_ptr<Attribute> topic_name_;
            //Not Yet implemented
            std::shared_ptr<Attribute> qos_path_;
            std::shared_ptr<Attribute> qos_name_;

            std::mutex writer_mutex_;
            dds::pub::DataWriter<RtiType> writer_ = dds::pub::DataWriter<RtiType>(dds::core::null);
   }; 
};


template <class BaseType, class RtiType>
rti::PublisherPort<BaseType, RtiType>::PublisherPort(std::weak_ptr<Component> component, std::string name): ::PublisherPort<BaseType>(component, name, "rti"){
    publisher_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "publisher_name").lock();
    domain_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "domain_id").lock();
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    qos_path_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_path").lock();
    qos_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "qos_profile_name").lock();

    //Set default publisher_name
    publisher_name_->set_String("In_" + this->get_name());
};


template <class BaseType, class RtiType>
void rti::PublisherPort<BaseType, RtiType>::HandleConfigure(){
    SetupWriter();
    ::PublisherPort<BaseType>::HandleConfigure();
};


template <class BaseType, class RtiType>
void rti::PublisherPort<BaseType, RtiType>::HandlePassivate(){
    DestructWriter();
    ::PublisherPort<BaseType>::HandlePassivate();
};


template <class BaseType, class RtiType>
void rti::PublisherPort<BaseType, RtiType>::HandleTerminate(){
    DestructWriter();
    ::PublisherPort<BaseType>::HandleTerminate();
};


template <class BaseType, class RtiType>
void rti::PublisherPort<BaseType, RtiType>::Send(const BaseType& message){
    //Log the recieving
    this->EventRecieved(message);

    if(this->is_running()){
        //std::lock_guard<std::mutex> lock(writer_mutex_);
        if(writer_ != dds::core::null){
            try{
                auto m = ::Base::Translator<BaseType, RtiType>::BaseToMiddleware(message);
                if(m){
                    //De-reference the message and send
                    writer_.write(*m);
                    this->EventProcessed(message);
                    this->logger().LogPortUtilizationEvent(*this, message, Logger::UtilizationEvent::SENT);
                    return;
                }
            }catch(const std::exception& ex){
                std::string error_str("Failed to Translate Message to publish: ");
                this->ProcessMessageException(message, error_str + ex.what());
            }
        }
    }
    this->EventIgnored(message);
};

template <class BaseType, class RtiType>
void rti::PublisherPort<BaseType, RtiType>::SetupWriter(){
    std::lock_guard<std::mutex> lock(writer_mutex_);
    if(writer_ == dds::core::null){
        //Construct a DDS Participant, Publisher, Topic and Writer
        auto& helper = get_dds_helper();   
        auto participant = helper.get_participant(domain_id_->Integer());
        auto topic = get_topic<RtiType>(participant, topic_name_->String());
        auto publisher = helper.get_publisher(participant, publisher_name_->String());
        writer_ = get_data_writer<RtiType>(publisher, topic, qos_path_->String(), qos_name_->String());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }else{
        throw std::runtime_error("rti Publisher Port: '" + this->get_name() + "': Has an errant writer!");
    }
};

template <class BaseType, class RtiType>
void rti::PublisherPort<BaseType, RtiType>::DestructWriter(){
    std::lock_guard<std::mutex> lock(writer_mutex_);
    if(writer_ != dds::core::null){
        writer_.close();
        writer_ = dds::pub::DataWriter<RtiType>(dds::core::null);
    }
};


#endif //RTI_PORT_PUBLISHER_HPP