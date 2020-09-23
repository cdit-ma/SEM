#include "monitor.h"
#include <iostream>
#include <functional>

zmq::Monitor::Monitor(zmq::socket_t& socket){
    future_ = std::async(std::launch::async, &Monitor::MonitorThread, this, std::ref(socket), ZMQ_EVENT_ALL);
}

zmq::Monitor::~Monitor(){
    abort_ = true;
    
    if(future_.valid()){
        future_.get();
    }
}


void zmq::Monitor::RegisterEventCallback(const uint8_t& event, EventCallbackFn fn){
    std::lock_guard<std::mutex> lock(callback_mutex_);
    callbacks_[event] = fn;
}

void zmq::Monitor::MonitorThread(std::reference_wrapper<zmq::socket_t> socket, const int event_type){
    static std::atomic<int> monitor_count{0};
    
    try{
        std::string monitor_address{"inproc://monitor_" + std::to_string(monitor_count++)};
        
        init(socket.get(), monitor_address.c_str());
        
        while(!abort_){
            check_event(1000);
        }
    }catch(const zmq::error_t& ex){
        std::cerr << "MonitorThread: " << ex.what() << std::endl;
    }
}

void zmq::Monitor::on_event(const zmq_event_t &event, const char* addr){
    std::lock_guard<std::mutex> lock(callback_mutex_);
    auto location = callbacks_.find(event.event);
    if(location != callbacks_.cend()){
        location->second(event.event, std::string(addr));
    }
}

void zmq::Monitor::on_event_connected(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_connect_delayed(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_connect_retried(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_listening(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_bind_failed(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_accepted(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_accept_failed(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_closed(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_close_failed(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_disconnected(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_handshake_failed(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_handshake_succeed(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
void zmq::Monitor::on_event_unknown(const zmq_event_t &event, const char* addr){
    on_event(event, addr);
}
