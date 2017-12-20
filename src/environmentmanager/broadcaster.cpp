#include "broadcaster.h"

Broadcaster::Broadcaster(const std::string& endpoint){
    endpoint_ = endpoint;
    context_ = new zmq::context_t(1);
}

void Broadcaster::SetEndpoint(const std::string& endpoint){
    endpoint_ = endpoint;
}

std::string Broadcaster::GetEndpoint() const{
    return endpoint_;
}

void Broadcaster::StartBroadcast(){
    broadcast_loop_ = new std::thread(&Broadcaster::BroadcastLoop, this);
}

void Broadcaster::EndBroadcast(){

    //condition var here

    broadcast_loop_->join();

}

zmq::message_t Broadcaster::GetMessage(){
    return zmq::message_t(endpoint_.begin(), endpoint_.end());
}

void Broadcaster::BroadcastLoop(){
    zmq::socket_t publisher(*context_, ZMQ_PUB);
    std::string bind_addr("tcp://*:" + broadcast_port_);
    publisher.bind(bind_addr);

    while(true){
        //add condition variable to break out of this boi
        std::string message_str(endpoint_);
        zmq::message_t message = GetMessage();
        publisher.send(message);

        std::cout << "bcast: " << message_str << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(broadcast_period_));
    }
}