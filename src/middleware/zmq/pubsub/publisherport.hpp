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
            PublisherPort(std::weak_ptr<Component> component, const std::string& name);
            ~PublisherPort(){this->Terminate();};
            void Send(const BaseType& message);
        protected:
            void HandleConfigure();
            void HandleActivate();
            void HandlePassivate();
            void HandleTerminate();
        private:
            void InterruptLoop();
            void Loop(ThreadManager& thread_manager, std::unique_ptr<zmq::socket_t> xsub_socket, std::unique_ptr<zmq::socket_t> xpub_socket);
            zmq::socket_t& GetSocket();
            
            std::mutex mutex_;
            std::unique_ptr<ThreadManager> thread_manager_;
            std::unique_ptr<zmq::context_t> context_;
            std::string proxy_endpoint_;

            std::shared_ptr<Attribute> end_points_;

            std::unordered_map<std::thread::id,  std::unique_ptr<zmq::socket_t> > sockets_;
    }; 
};

template <class BaseType, class ProtoType>
zmq::PublisherPort<BaseType, ProtoType>::PublisherPort(std::weak_ptr<Component> component, const std::string& port_name):
    ::PublisherPort<BaseType>(component, port_name, "zmq"){

    const auto component_ = component.lock();
    const auto component_name = component_ ? component_->get_name() : "??";
    const auto component_id = component_ ? component_->get_id() : "??";
    const auto port_id = ::Activatable::get_id();

    proxy_endpoint_ = "inproc://" + component_name + "*" + component_id + "*" + port_name + "*" + port_id;
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
};

template <class BaseType, class ProtoType>
zmq::socket_t& zmq::PublisherPort<BaseType, ProtoType>::GetSocket(){
    const auto& thread_id = std::this_thread::get_id();
    
    std::unique_lock<std::mutex> lock(mutex_);
    auto add_socket = sockets_.count(thread_id) == 0;
    
    if(add_socket){
        if(!context_)
            throw std::runtime_error("ZMQ Publisher Port: '" + this->get_name() + "': Has an no ZMQ Context!");
        
        auto& helper = get_zmq_helper();
        auto socket = helper.get_socket(*context_, ZMQ_PUB);
        try{
            socket->connect(proxy_endpoint_.c_str());
        }catch(const zmq::error_t& ex){
            throw std::runtime_error("ZMQ Publisher Port: '" + this->get_name() + "': Failed to connect to: '" + proxy_endpoint_);
        }
        sockets_[thread_id] = std::move(socket);
    }
    auto& socket = *sockets_[thread_id];
    lock.unlock();

    if(add_socket){
        //Sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return socket;
}


template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(thread_manager_)
        throw std::runtime_error("ZMQ Publisher Port: '" + this->get_name() + "': Has an errant ThreadManager!");
    if(context_)
        throw std::runtime_error("ZMQ Publisher Port: '" + this->get_name() + "': Has an errant ZMQ Context!");

    auto& helper = get_zmq_helper();
    context_ = helper.get_context();

    //Get a subscriber port
    auto xsub_socket = helper.get_socket(*context_, ZMQ_XSUB);
    auto xpub_socket = helper.get_socket(*context_, ZMQ_XPUB);

    try{
        xsub_socket->bind(proxy_endpoint_.c_str());
    }catch(const zmq::error_t& ex){
        throw std::runtime_error("ZMQ Publisher Port: '" + this->get_name() + "': Failed to bind: '" + proxy_endpoint_);
    }

    for(const auto& endpoint : end_points_->StringList()){
        try{
            xpub_socket->bind(endpoint.c_str());
        }catch(const zmq::error_t& ex){
            throw std::runtime_error("ZMQ Publisher Port: '" + this->get_name() + "': Failed to bind: '" + endpoint);
        }
    }
    //
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    thread_manager_ = std::unique_ptr<ThreadManager>(new ThreadManager());
    auto future = std::async(std::launch::async, &zmq::PublisherPort<BaseType, ProtoType>::Loop, this, std::ref(*thread_manager_), std::move(xsub_socket), std::move(xpub_socket));
    thread_manager_->SetFuture(std::move(future));
    thread_manager_->Configure();
    ::PublisherPort<BaseType>::HandleConfigure();
};

template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::HandleActivate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!thread_manager_)
        throw std::runtime_error("ZMQ Publisher Port: '" + this->get_name() + "': Has no ThreadManager!");

    thread_manager_->Activate();
    ::PublisherPort<BaseType>::HandleActivate();
};

template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::HandlePassivate(){
    InterruptLoop();
    ::PublisherPort<BaseType>::HandlePassivate();
}


template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::HandleTerminate(){
    InterruptLoop();
    std::lock_guard<std::mutex> lock(mutex_);
    
    thread_manager_.reset();
    ::PublisherPort<BaseType>::HandleTerminate();
};


template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::Send(const BaseType& message){
    
    //Log the recieving
    this->EventRecieved(message);

    if(this->is_running()){
        auto& socket = GetSocket();
        
        //Translate the base_request object into a string
        const auto request_str = ::Proto::Translator<BaseType, ProtoType>::BaseToString(message);
        socket.send(String2Zmq(request_str));
        this->EventProcessed(message);
        this->logger().LogComponentEvent(*this, message, ModelLogger::ComponentEvent::SENT);
        return;
    }
    this->EventIgnored(message);
};

template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::InterruptLoop(){
    std::lock_guard<std::mutex> lock(mutex_);
    //Free the context, which will block until all socket_t's are released
    if(thread_manager_){
        thread_manager_->SetTerminate();
    }
    sockets_.clear();
    context_.reset();
};

template <class BaseType, class ProtoType>
void zmq::PublisherPort<BaseType, ProtoType>::Loop(ThreadManager& thread_manager, std::unique_ptr<socket_t> xsub_socket, std::unique_ptr<socket_t> xpub_socket){
    thread_manager.Thread_Configured();

    if(thread_manager.Thread_WaitForActivate()){
        ::Port::LogActivation();
        thread_manager.Thread_Activated();
        try{
            zmq::proxy((void*)*(xsub_socket.get()), (void*)*(xpub_socket.get()), nullptr);
        }catch(const zmq::error_t& ex){
            if(ex.num() != ETERM){
                std::cerr << "zmq::PublisherPort: '" + this->get_name() + "' " << ex.what() << std::endl;
            }
        }
        ::Port::LogPassivation();
    }
    thread_manager.Thread_Terminated();
};

#endif //ZMQ_PORT_PUBLISHER_HPP

