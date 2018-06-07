#ifndef ZMQ_PORT_REPLIER_HPP
#define ZMQ_PORT_REPLIER_HPP

#include <core/ports/requestreply/replierport.hpp>
#include <middleware/proto/prototranslator.h>
#include <middleware/zmq/zmqhelper.h>
#include <re_common/zmq/zmqutils.hpp>

#include <thread>
#include <mutex>

namespace zmq{
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    struct RequestHandler;

    //Generic templated ReplierPort
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    class ReplierPort : public ::ReplierPort<BaseReplyType, BaseRequestType>{
        //The Request Handle needs to be able to modify and change state of the Port
        friend class RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>;
        
        public:
            ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > server_function);
            ~ReplierPort(){
                Activatable::Terminate();
            };

            using middleware_reply_type = ProtoReplyType;
            using middleware_request_type = ProtoRequestType;
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            bool TerminateThread();
            
            std::thread* recv_thread_ = 0;
            std::shared_ptr<Attribute> end_point_;

            std::mutex thread_state_mutex_;
            ThreadState thread_state_ = ThreadState::TERMINATED;
            std::condition_variable thread_state_condition_;
            
            std::mutex control_mutex_;
            
            std::string terminate_endpoint_;
            const std::string terminate_str = "TERMINATE";
    };

    //Generic templated RequesterHandler
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    struct RequestHandler{
        static void Loop(zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port);
    };
    
    //Specialised templated RequesterHandler for void returning
    template <class BaseRequestType, class ProtoRequestType>
    struct RequestHandler<void, void, BaseRequestType, ProtoRequestType>{
        static void Loop(zmq::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port);
    };
};

//Generic templated ReplierPort
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name,  std::function<BaseReplyType (BaseRequestType&) > server_function):
::ReplierPort<BaseReplyType, BaseRequestType>(component, port_name, server_function, "zmq"){
    auto component_ = component.lock();
    auto component_name = component_ ? component_->get_name() : "??";
    auto component_id = component_ ? component_->get_id() : "??";
    terminate_endpoint_ = "inproc://term*" + component_name + "*" + port_name + "*" + component_id + "*";

    end_point_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = end_point_->String().size() >= 0;
    if(valid && ::ReplierPort<BaseReplyType, BaseRequestType>::HandleConfigure()){
        if(!recv_thread_){
            //Construct the thread, and wait for the thread state to be changed
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            recv_thread_ = new std::thread(zmq::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop, std::ref(*this));
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Call into the base class
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandleTerminate()){
        //Terminate the thread
        TerminateThread();

        if(recv_thread_){
            //Join our zmq_thread
            recv_thread_->join();
            delete recv_thread_;
            recv_thread_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    //Call into the base Handle Passivate
    if(::ReplierPort<BaseReplyType, BaseRequestType>::HandlePassivate()){
        return TerminateThread();
    }
    return false;
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::TerminateThread(){
    ThreadState state;
    {
        std::lock_guard<std::mutex> lock(thread_state_mutex_);
        state = thread_state_;
    }


    switch(state){

        case ThreadState::ACTIVE:{
            try{
                auto socket = ZmqHelper::get_zmq_helper()->get_request_socket();
            
                //Connect to the terminate address
                socket.connect(terminate_endpoint_.c_str());
                
                //Send the special terminate message
                zmq::message_t term_msg(terminate_str.c_str(), terminate_str.size());
                
                socket.send(term_msg);
                //Wait for the response
                std::cerr << "WIAITING " << std::endl;
                socket.recv(&term_msg);
                return true;
            }catch(const zmq::error_t& ex){
                Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to Terminate ZMQ Server Port") + ex.what());
            }
            break;
        }
        case ThreadState::STARTED:{
            return true;
        }
        case ThreadState::TERMINATED:{
            return true;
        }
        default:
            break;
    }
    return false;
}


//Generic templated RequestHandler
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void zmq::RequestHandler<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::Loop(zmq::ReplierPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>& port){
    auto socket = ZmqHelper::get_zmq_helper()->get_reply_socket();
    auto state = ThreadState::STARTED;

    const auto& terminate_address = port.terminate_endpoint_;
    try{
        //Bind the Terminate address
        socket.bind(terminate_address.c_str());
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind terminate endpoint: '" + terminate_address + "' " + ex.what());
        state = ThreadState::TERMINATED;
    }

    const auto address = port.end_point_->String();
    try{
        //Bind the Outgoing address
        socket.bind(address.c_str());
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind endpoint: '" + address + "' " + ex.what());
        state = ThreadState::TERMINATED;
    }

    {
        //Update the Port's state variable 
        std::lock_guard<std::mutex> lock(port.thread_state_mutex_);
        port.thread_state_ = state;
        port.thread_state_condition_.notify_all();
    }

    if(state == ThreadState::STARTED && port.BlockUntilStateChanged(Activatable::State::RUNNING)){
        {
            //Update the Port's state variable 
            std::lock_guard<std::mutex> lock(port.thread_state_mutex_);
            port.thread_state_ = ThreadState::ACTIVE;
        }
        //Log the port becoming online
        port.LogActivation();

        while(true){
            try{
                //Wait for next message
                zmq::message_t zmq_request;
                socket.recv(&zmq_request);

                const auto& request_str = Zmq2String(zmq_request);
                
                if(request_str == port.terminate_str){
                    //Send back a Terminate
                    socket.send(zmq_request);
                    break;
                }
                
                auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(request_str);
                
                //Call through the base ProcessRequest function, which calls any attached callback
                auto base_reply = port.ProcessRequest(*base_request_ptr);
                auto reply_str = ::Proto::Translator<BaseReplyType, ProtoReplyType>::BaseToString(base_reply);
                
                //Send reply
                socket.send(String2Zmq(reply_str));
            }catch(const zmq::error_t& ex){
                Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                break;
            }
        }
        
        //Log that the port has been passivated
        port.LogPassivation();
    }
    
    //Change the state to be Terminated
    std::lock_guard<std::mutex> lock(port.thread_state_mutex_);
    port.thread_state_ = ThreadState::TERMINATED;
};

//Specialised templated RequestHandler for void returning
template <class BaseRequestType, class ProtoRequestType>
void zmq::RequestHandler<void, void, BaseRequestType, ProtoRequestType>::Loop(zmq::ReplierPort<void, void, BaseRequestType, ProtoRequestType>& port){
    auto socket = ZmqHelper::get_zmq_helper()->get_reply_socket();
    auto state = ThreadState::STARTED;

    const auto& terminate_address = port.terminate_endpoint_;
    try{
        //Bind the Terminate address
        socket.bind(terminate_address.c_str());
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind terminate endpoint: '" + terminate_address + "' " + ex.what());
        state = ThreadState::TERMINATED;
    }

    const auto address = port.end_point_->String();
    try{
        //Bind the Outgoing address
        socket.bind(address.c_str());
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(&port).Func(__func__).Msg("Cannot bind endpoint: '" + address + "' " + ex.what());
        state = ThreadState::TERMINATED;
    }

    {
        //Update the Port's state variable 
        std::lock_guard<std::mutex> lock(port.thread_state_mutex_);
        port.thread_state_ = state;
        port.thread_state_condition_.notify_all();
    }

    if(state == ThreadState::STARTED && port.BlockUntilStateChanged(Activatable::State::RUNNING)){
        //Log the port becoming online
        port.LogActivation();

        while(true){
            try{
                //Wait for next message
                zmq::message_t zmq_request;
                socket.recv(&zmq_request);

                const auto& request_str = Zmq2String(zmq_request);
                
                if(request_str == port.terminate_str){
                    //Send back a Terminate
                    socket.send(zmq_request);
                    break;
                }
                
                auto base_request_ptr = ::Proto::Translator<BaseRequestType, ProtoRequestType>::StringToBase(request_str);
                
                //Call through the base ProcessRequest function, which calls any attached callback
                port.ProcessRequest(*base_request_ptr);

                //Send blank response
                socket.send(String2Zmq(""));
            }catch(const zmq::error_t& ex){
                Log(Severity::ERROR_).Context(&port).Func(__func__).Msg(ex.what());
                break;
            }
        }
        
        //Log that the port has been passivated
        port.LogPassivation();
    }

    //Change the state to be Terminated
    std::lock_guard<std::mutex> lock(port.thread_state_mutex_);
    port.thread_state_ = ThreadState::TERMINATED;
}



#endif //ZMQ_PORT_REPLIER_HPP
