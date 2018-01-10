#include "broadcaster.h"

Broadcaster::Broadcaster(const std::string& endpoint, const std::string& message){
    endpoint_ = endpoint;
    message_ = message;
    context_ = new zmq::context_t(1);
}

void Broadcaster::SetMessage(const std::string& message){
    message_ = message;
}

std::string Broadcaster::GetMessage() const{
    return message_;
}

void Broadcaster::StartBroadcast(){
    broadcast_loop_ = new std::thread(&Broadcaster::BroadcastLoop, this);
}

void Broadcaster::EndBroadcast(){

    terminate_flag_ = true;
    //TODO: condition var here

    broadcast_loop_->join();
}

zmq::message_t Broadcaster::GetZMQMessage(){
    // TODO: Format message nicely?
    return zmq::message_t(message_.begin(), message_.end());
}

void Broadcaster::BroadcastLoop(){
    zmq::socket_t publisher(*context_, ZMQ_PUB);

    try{
        publisher.bind(endpoint_);
    }
    catch(zmq::error_t& e){
        //TODO: Handle this better.
        throw new std::invalid_argument("Could not bind broadcast endpoint in Broadcast loop. Endpoint: " + endpoint_);
    }

    while(!terminate_flag_){
        //TODO: add condition variable to break out of this
        zmq::message_t message = GetZMQMessage();
        publisher.send(message);

        std::this_thread::sleep_for(std::chrono::seconds(broadcast_period_));
    }
}