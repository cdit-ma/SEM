#include "monitor.h"

zmq::Monitor::Monitor(){
}

zmq::Monitor::~Monitor(){
    if(monitor_thread_){
        //Abort the thread
        abort();
        monitor_thread_->join();
        delete monitor_thread_;
    }
}

void zmq::Monitor::RegisterEventCallback(std::function<void(int, std::string)> fn){
    callback_ = fn;
}

void zmq::Monitor::MonitorSocket(zmq::socket_t* socket, std::string address, int event_type){
    if(socket && !monitor_thread_){
        //Call a thread to run monitor
        monitor_thread_ = new std::thread([=]{monitor(*socket, address.c_str(), event_type);});
    }
}

void zmq::Monitor::on_event(const zmq_event_t &event, const char* addr){
    std::string addr_str(addr);
    int event_type = event.event;
    if(callback_){
        callback_(event_type, addr_str);
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
