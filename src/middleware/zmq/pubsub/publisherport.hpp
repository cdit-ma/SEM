#ifndef ZMQ_PORT_PUBLISHER_HPP
#define ZMQ_PORT_PUBLISHER_HPP

#include <thread>

#include <core/ports/pubsub/publisherport.hpp>
#include <middleware/proto/prototranslator.h>
#include <middleware/zmq/zmqhelper.h>
#include <re_common/zmq/zmqutils.hpp>


namespace zmq{
    template <class BaseType, class ProtoType>
     class PublisherPort : public ::PublisherPort<BaseType>{
        public:
            PublisherPort(std::weak_ptr<Component> component, std::string name);
            ~PublisherPort(){
                Activatable::Terminate();
            };
            
        protected:
            bool HandleConfigure();
            bool HandleTerminate();
        public:
            bool Send(const BaseType& t);
        private:
            bool setupTx();
            std::shared_ptr<Attribute> end_points_;
            std::mutex socket_mutex;
            std::unique_ptr<zmq::socket_t> socket;
    }; 
};

template <class BaseType, class ProtoType>
zmq::PublisherPort<BaseType, ProtoType>::PublisherPort(std::weak_ptr<Component> component, std::string name): ::PublisherPort<BaseType>(component, name, "zmq"){
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
};


template <class BaseType, class ProtoType>
bool zmq::PublisherPort<BaseType, ProtoType>::HandleTerminate(){
    if(::PublisherPort<BaseType>::HandleTerminate()){
        std::lock_guard<std::mutex> lock(socket_mutex);
        if(socket){
            auto raw_socket = socket.release();
            delete raw_socket;

        }
        return true;
    }
    return false;
};

template <class BaseType, class ProtoType>
bool zmq::PublisherPort<BaseType, ProtoType>::HandleConfigure(){
    if(::PublisherPort<BaseType>::HandleConfigure()){
        return setupTx();
    }
    return false;
};


template <class BaseType, class ProtoType>
bool zmq::PublisherPort<BaseType, ProtoType>::setupTx(){
    std::lock_guard<std::mutex> lock(socket_mutex);
    if(!socket){
        auto helper = ZmqHelper::get_zmq_helper();
        socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(std::move(helper->get_publisher_socket() ) ));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        for(const auto& endpoint : end_points_->StringList()){
            try{
                //connect the addresses provided
                socket->bind(endpoint.c_str());
                //std::cerr << "BINDING: " << endpoint << std::endl;
            }catch(zmq::error_t ex){
                Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to endpoint: '" + endpoint + "' " + ex.what());
                return false;
            }
        }
        //XXX: DO NOT REMOVE THE SLEEP ZMQ NEEDS HIS REST
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return true;
};

template <class BaseType, class ProtoType>
bool zmq::PublisherPort<BaseType, ProtoType>::Send(const BaseType& message){
    bool should_send = ::PublisherPort<BaseType>::Send(message);

    if(should_send){
        
        

        //Translate the base_request object into a string
        const auto request_str = ::Proto::Translator<BaseType, ProtoType>::BaseToString(message);
        //Send the request
        socket->send(String2Zmq(request_str));
        return true;
    }
    return false;
};

#endif //ZMQ_PORT_PUBLISHER_HPP

