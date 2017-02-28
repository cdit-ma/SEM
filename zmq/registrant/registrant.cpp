#include "registrant.h"
#include <iostream>
#include <zmq.hpp>

template<typename T>
inline void remove(std::vector<T> & v, const T & item)
{
    v.erase(std::remove(v.begin(), v.end(), item), v.end());
}

zmq::Registrant::Registrant(std::string request_message){
    this->request_message_ = request_message;
    
    //Construct the context so we can terminate easily
    context_ = new zmq::context_t(1);
};

void zmq::Registrant::Start(){
    std::unique_lock<std::mutex> lock(mutex_);

    //Only allow once
    if(thread_queue_.empty()){
        //Connect to all endpoints
        for(std::string endpoint: endpoints_){
            auto t = new std::thread(&zmq::Registrant::RegistrationThread, this, endpoint);
            thread_queue_.push(t);
            std::cout << "STARTED THREAD: " << t << " # " << thread_queue_.size() << std::endl;
        }
    }
}

void zmq::Registrant::AddEndpoint(std::string endpoint){
    //Gain the lock so we can push this message onto our queue
    std::unique_lock<std::mutex> lock(mutex_);
    //Push back onto queue
    endpoints_.push_back(endpoint);
}

zmq::Registrant::~Registrant(){
    //Deleting context will interupt the socket's blocked send/recv
    std::cout << "DELETING CONTEXT_" << std::endl;
    delete context_;
    std::cout << "DELETED CONTEXT_" << std::endl;
    //Clean up all threads
    while(!thread_queue_.empty()){
        std::cout << "WAITING FOR THREADS!" << std::endl;
        auto t = thread_queue_.front();
        std::cout << "WAITING ON THREAD: " << t << " # " << thread_queue_.size() << std::endl;
        if(t){
            t->join();
            delete t;
        }
        thread_queue_.pop();
    }
}

void zmq::Registrant::RegisterNotify(std::function<void(std::string, std::string)> fn){
    callback_ = fn;
}

void zmq::Registrant::RegistrationThread(std::string endpoint){
    //Construct a Request zmq_socket
    auto socket = new zmq::socket_t(*context_, ZMQ_REQ);
    //Set Linger to 0
    int linger = 0;
    socket->setsockopt(ZMQ_LINGER, &linger, sizeof (linger));

    try{
        socket->connect(endpoint);
    }
    catch(const zmq::error_t& exception){
        std::cerr << "Cannot Connect to Address!" << std::endl;
        std::cerr << "ZMQ Error: " << exception.what() << std::endl;
    }

    while(true){
        try{
            zmq::message_t registrant_id(request_message_.c_str(), request_message_.size());
            zmq::message_t registrant_topic_name;
            zmq::message_t registrar_message;
            
            //Send the request_message
            socket->send(registrant_id);

            //Wait for the Registrar to reply with it's endpoint, then Response
            socket->recv(&registrant_topic_name);
            socket->recv(&registrar_message);

            //Convert the recieved messages to strings
            std::string registrant_topic_name_str(static_cast<char *>(registrant_topic_name.data()), registrant_topic_name.size());
            std::string registrar_message_str(static_cast<char *>(registrar_message.data()), registrar_message.size());
            
            if(callback_){
                //Call into the callback function with the topic_name and response
                callback_(registrant_topic_name_str, registrar_message_str);
            }
            //We've done
            break;
        }catch(const zmq::error_t& exception){
            if(exception.num() == ETERM){
                std::cout << "Terminating Registration Thread!" << std::endl;
            }else{
                std::cerr << "ZMQ::Registrant::RegistrationThread: " << exception.what() << std::endl;
            }
            break;
        }
    }
    delete socket;
    std::cout << "THREAD FINISHED" << std::endl;
}