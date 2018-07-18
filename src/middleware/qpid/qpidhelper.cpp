#include "qpidhelper.h"

qpid::QpidHelper& qpid::QpidHelper::get_qpid_helper(){
    static qpid::QpidHelper q;
    return q;
};

qpid::PortHelper::PortHelper(const std::string& broker_endpoint){
    //Construct qpid connection and session with broker info and constructed topic name
    connection_ = qpid::messaging::Connection(broker_endpoint);
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

        
    return session_.createSender("amq.topic/" + topic_name);//";{create:always, delete:always, node:{type:queue,x-declare:{arguments:{'qpid.last_value_queue_key':sender}}}}");
}

qpid::messaging::Receiver qpid::PortHelper::GetReceiver(const std::string& topic_name){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!session_)
        throw std::runtime_error("Not QPid Session");

    return session_.createReceiver("amq.topic/" + topic_name);// + ";{create:always, delete:always, node:{type:queue,x-declare:{arguments:{'qpid.last_value_queue_key':sender}}}}");
}

qpid::messaging::Sender qpid::PortHelper::GetSender(const qpid::messaging::Address& address){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!session_)
        throw std::runtime_error("Not QPid Session");
    return session_.createSender(address);
}

qpid::messaging::Receiver qpid::PortHelper::GetReceiver(const qpid::messaging::Address& address){
    std::lock_guard<std::mutex> lock(mutex_);
    if(!session_)
        throw std::runtime_error("Not QPid Session");
    return session_.createReceiver(address);
}