#include "registrar.h"
#include <iostream>
#include <zmq.hpp>

zmq::Registrar::Registrar(std::string endpoint, std::string reply_message){
    //Store the endpoint and message to send Registrants
    this->endpoint_ = endpoint;
    this->reply_message_ = reply_message;
    
    //Construct the context so we can terminate easily
    context_ = new zmq::context_t(1);
};


void zmq::Registrar::Start(){
    if(!register_thread_ && !notify_thread_){
        register_thread_ = new std::thread(&zmq::Registrar::RegistrationThread, this);
        notify_thread_ = new std::thread(&zmq::Registrar::NotifyThread, this);
    }
}

zmq::Registrar::~Registrar(){
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        terminate_ = true;
        queue_lock_condition_.notify_all();
    }
        
    //Wait for the notify thread to be joined
    if(notify_thread_){
        notify_thread_->join();
        delete notify_thread_;
    }

    if(context_){
        //Deleting the ZMQ context will terminate the socket in the register_thread
        delete context_;

        if(register_thread_){
            register_thread_->join();
            delete register_thread_;
        }
    }
};

void zmq::Registrar::RegisterNotify(std::function<void(std::string)> fn){
    callback_ = fn;
}

void zmq::Registrar::NotifyThread(){
    while(true){
        std::queue<std::string> replace_queue;
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait for the condition to be notified
            queue_lock_condition_.wait(lock);
            
            //Check if we should terminate
            if(terminate_){
                return;
            }else{
                //Now that we have access, swap out queues and release the mutex
                if(!registered_endpoints_.empty()){
                    registered_endpoints_.swap(replace_queue);
                }
            }
        }
        //Process the queue
        while(!replace_queue.empty()){
            std::cout << "Registrar: Got: " << replace_queue.front() << std::endl;
            if(callback_){
                std::cout << "CALLING IN TO CALLBACK!" << std::endl;
                callback_(replace_queue.front());
            }
            replace_queue.pop();
        }
    }
}

void zmq::Registrar::RegistrationThread(){
    auto socket = zmq::socket_t(*context_, ZMQ_REP);

    //Try bind
    try{
        socket.bind(endpoint_.c_str());
    }
    catch(const zmq::error_t& exception){
        std::cerr << "Cannot Bind to Address: " << endpoint_.c_str() << std::endl;
        std::cerr << "ZMQ Error: " << exception.what() << std::endl;
    }
    
    //Message object to fill
    while(true){
        try{
            zmq::message_t registrant_addr;
            //Send the Registrant the Registrar endpoint, then reply message
            zmq::message_t addr(endpoint_.c_str(), endpoint_.size());
            zmq::message_t message(reply_message_.c_str(), reply_message_.size());
            
            //Wait for Registrant to request
            socket.recv(&registrant_addr);

            
            socket.send(addr, ZMQ_SNDMORE);
            socket.send(message);

            std::string registrant_addr_str(static_cast<char *>(registrant_addr.data()), registrant_addr.size());

            //Gain the lock so we can push this message onto our queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            std::cout << registrant_addr_str << std::endl;
            registered_endpoints_.push(registrant_addr_str);
            //Call into notify thread
            queue_lock_condition_.notify_all();
        }catch(const zmq::error_t& exception){
            if(exception.num() == ETERM){
                std::cout << "Terminating Registration Thread!" << std::endl;
            }
            return;
        }
    }
}