#include "qpidhelper.h"

qpid::QpidHelper& qpid::QpidHelper::get_qpid_helper(){
    static qpid::QpidHelper q;
    return q;
};

qpid::PortHelper::PortHelper(const std::string& broker_endpoint){
    //Construct qpid connection and session with broker info and constructed topic name
    connection_ = qpid::messaging::Connection(broker_endpoint->String());
    connection_.open();
    session_ = connection_.createSession();
}

qpid::PortHelper::~PortHelper(){
    Terminate();
}

void qpid::PortHelper::Terminate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(connection_ && connection_.isOpen()){
        connection_.close();
        connection_ = 0;
        session_ = 0;
    }
}

qpid::messaging::Sender qpid::PortHelper::GetSender(const std::string& topic_name){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!session_)
        throw std::runtime_error("Not QPid Session");
    auto sender = session_.createSender("amq.topic/reqrep/"  + topic_name);
    return std::move(sender);
}

qpid::messaging::Receiver qpid::PortHelper::Receiver(const std::string& topic_name){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!session_)
        throw std::runtime_error("Not QPid Session");
    
    auto response_queue_address = qpid::messaging::Address("#response-queue; {create: always, delete:always}");
    auto receiver = session_.createReceiver(response_queue_address);
    return std::move(receiver);
}