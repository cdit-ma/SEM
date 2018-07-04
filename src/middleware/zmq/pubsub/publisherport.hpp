#ifndef ZMQ_PORT_PUBLISHER_HPP
#define ZMQ_PORT_PUBLISHER_HPP

#include <core/ports/pubsub/publisherport.hpp>
#include <middleware/proto/prototranslator.h>
#include <middleware/zmq/zmqhelper.h>
#include <re_common/zmq/zmqutils.hpp>
#include <core/modellogger.h>


namespace zmq{
    template <class BaseType, class ProtoType>
     class PublisherPort : public ::PublisherPort<BaseType>{
        public:
            PublisherPort(std::weak_ptr<Component> component, std::string name);
            ~PublisherPort(){this->Terminate();};
            void Send(const BaseType& message);
        protected:
            void HandleConfigure();
            void HandleTerminate();
        private:
            void SetupTx();

            std::mutex socket_mutex;
            std::shared_ptr<Attribute> end_points_;
            std::unique_ptr<zmq::socket_t> socket_;
    }; 
};

template <class BaseType, class ProtoType>
zmq::PublisherPort<BaseType, ProtoType>::PublisherPort(std::weak_ptr<Component> component, std::string name): ::PublisherPort<BaseType>(component, name, "zmq"){
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
};


template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(socket_mutex);
    socket_.reset();
    ::PublisherPort<BaseMessage>::HandleTerminate();
    this->logger().LogLifecycleEvent(*this, ModelLogger::LifeCycleEvent::TERMINATED);
};

template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::HandleConfigure(){
    SetupTx();
    ::PublisherPort<BaseMessage>::HandleConfigure();
};


template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::SetupTx(){
    std::lock_guard<std::mutex> lock(socket_mutex);
    if(!socket_){
        auto& helper = ZmqHelper::get_zmq_helper();
        socket_ = helper.get_publisher_socket();
        
        for(const auto& endpoint : end_points_->StringList()){
            try{
                socket_->bind(endpoint.c_str());
            }catch(zmq::error_t ex){
                throw std::runtime_error("Cannot connect to endpoint: '" + endpoint + "' " + ex.what());
            }
        }
        //XXX: DO NOT REMOVE THE SLEEP ZMQ NEEDS HIS REST
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
};

template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::Send(const BaseType& message){
    //Log the recieving
    this->EventRecieved(message);

    if(this->is_running()){
        std::lock_guard<std::mutex> lock(socket_mutex);
        if(socket_){
            //Translate the base_request object into a string
            const auto request_str = ::Proto::Translator<BaseType, ProtoType>::BaseToString(message);
            //Send the request
            socket_->send(String2Zmq(request_str));
            this->EventProcessed(message);
            this->logger().LogComponentEvent(*this, message, ModelLogger::ComponentEvent::SENT);
            return;
        }
    }
    this->EventIgnored(message);
};

#endif //ZMQ_PORT_PUBLISHER_HPP

