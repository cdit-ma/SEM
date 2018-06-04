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
            }
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        public:
            bool Send(const BaseType& t);
        private:
            bool setup_tx();
            std::mutex control_mutex_;

            ::Proto::Translator<BaseType, ProtoType> translater;
            
            zmq::socket_t* socket_ = 0;
            std::shared_ptr<Attribute> end_points_;
    }; 
};

template <class BaseType, class ProtoType>
zmq::PublisherPort<BaseType, ProtoType>::PublisherPort(std::weak_ptr<Component> component, std::string name): ::PublisherPort<BaseType>(component, name, "zmq"){
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
};



template <class BaseType, class ProtoType>
bool zmq::PublisherPort<BaseType, ProtoType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool valid = end_points_->StringList().size() > 0;

    if(valid && ::PublisherPort<BaseType>::HandleConfigure()){
        return setup_tx();
    }
    return false;
};

template <class BaseType, class ProtoType>
bool zmq::PublisherPort<BaseType, ProtoType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::PublisherPort<BaseType>::HandlePassivate()){
        if(socket_){
            delete socket_;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            socket_ = 0;

        }
        return true;
    }
    return false;
};

template <class BaseType, class ProtoType>
bool zmq::PublisherPort<BaseType, ProtoType>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    return ::PublisherPort<BaseType>::HandleTerminate();
};

template <class BaseType, class ProtoType>
bool zmq::PublisherPort<BaseType, ProtoType>::setup_tx(){
    auto helper = ZmqHelper::get_zmq_helper();
    this->socket_ = helper->get_publisher_socket();
    for(auto e: end_points_->StringList()){
        try{
            //Bind the addresses provided
            this->socket_->bind(e.c_str());
        }catch(zmq::error_t ex){
            Log(Severity::ERROR_).Context(this).Func(GET_FUNC).Msg("Cannot bind endpoint: '" + e + "' " + ex.what());
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
};

template <class BaseType, class ProtoType>
bool zmq::PublisherPort<BaseType, ProtoType>::Send(const BaseType& message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool should_send = ::PublisherPort<BaseType>::Send(message);

    if(should_send){
        if(socket_){
            auto str = translater.BaseToString(message);
            zmq::message_t data(str.c_str(), str.size());
            socket_->send(data);
            return true;
        }else{
            Log(Severity::DEBUG).Context(this).Func(GET_FUNC).Msg("Socket unexpectedly null");
        }
    }
    return false;
};

#endif //ZMQ_PORT_PUBLISHER_HPP

