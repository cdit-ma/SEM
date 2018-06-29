/* re_common
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "re_common"
 *
 * "re_common" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "re_common" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#include "monitor.h"

zmq::Monitor::Monitor(){
}

zmq::Monitor::~Monitor(){
    if(monitor_future_.valid()){
        //Abort the thread
        abort();
        monitor_future_.get();
    }
}

void zmq::Monitor::RegisterEventCallback(std::function<void(int, std::string)> fn){
    callback_ = fn;
}

bool zmq::Monitor::MonitorSocket(zmq::socket_t* socket, const std::string& address, int event_type){
    if(socket && !monitor_future_.valid()){
        monitor_future_ = std::async(std::launch::async, [=](){
            try{
                monitor(*socket, address.c_str(), event_type);
            }catch(const zmq::error_t& ex){

            }
        });
        return true;
    }
    return false;
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
