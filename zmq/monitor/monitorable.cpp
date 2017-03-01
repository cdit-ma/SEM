#include "monitorable.h"
#include "monitor.h"

//Declare our Static variables
int zmq::Monitorable::count_ = 0;
std::mutex zmq::Monitorable::mutex_;

std::string zmq::Monitorable::GetNewMonitorAddress(){
    std::string addr = "inproc://Monitor_" + std::to_string(GetNewMonitorID());
    return addr;
}

int zmq::Monitorable::GetNewMonitorID(){
    std::unique_lock<std::mutex> lock(zmq::Monitorable::mutex_);
    return ++count_;
}