#include "zmqslave.h"
#include <iostream>
#include <chrono>
#include "../modellogger.h"
ZMQSlave::ZMQSlave(DeploymentManager* manager, std::string endpoint){
    deployment_manager_ = manager;
    context_ = new zmq::context_t(1);
    endpoint_ = endpoint;

<<<<<<< HEAD
    logger_endpoint_ = "tcp://192.168.111.86:8000";
=======
    logger_endpoint_ = "tcp://192.168.111.87:8000";
>>>>>>> 39f8fdb152f8782fca2c30272ba577013c167af2

    //Start the registration thread
    registration_thread_ = new std::thread(&ZMQSlave::RegistrationLoop, this);
}

ZMQSlave::~ZMQSlave(){
    {
        //Unblock our mutex condition guarding
        std::unique_lock<std::mutex> lock(queue_mutex_);
        terminating = true;
        queue_lock_condition_.notify_all();
    }

    //Deleting the context will interupt any blocking ZMQ calls
    if(context_){
        delete context_;    
    }

    if(registration_thread_){
        registration_thread_->join();
        delete registration_thread_;
    }

    if(action_thread_){
        action_thread_->join();
        delete action_thread_;
    }
}

void ZMQSlave::RegistrationLoop(){
    auto socket = zmq::socket_t(*context_, ZMQ_REQ);
    socket.bind(endpoint_.c_str());

    //Wait for a period of time before trying to send
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //Construct a message to send to the server
    zmq::message_t slave_addr(endpoint_.c_str(), endpoint_.size());
    zmq::message_t server_addr;
    zmq::message_t slave_name;

    try{
        //Send our address to the server, blocks until reply
        socket.send(slave_addr);

        //Get the tcp address for the action publisher.
        socket.recv(&server_addr);

        //Get the tcp address for the action publisher.
        socket.recv(&slave_name);


        //Cast the ZMQ response to a std::string
        master_server_address_ = std::string(static_cast<char *>(server_addr.data()), server_addr.size());
        host_name_ = std::string(static_cast<char *>(slave_name.data()), slave_name.size());
        
        std::cout << "Got Action Publisher End-Point: " << master_server_address_ << std::endl;
        std::cout << "Got Slave hostname: " << host_name_ << std::endl;

        //Start the action subscriber loop
        reader_thread_ = new std::thread(&ZMQSlave::ActionSubscriberLoop, this);
        //Start the action loop
        action_thread_ = new std::thread(&ZMQSlave::ActionQueueLoop, this);
    }catch(const zmq::error_t& exception){
        if(exception.num() == ETERM){
            std::cout << "Terminating!" << std::endl;
        }
    }
}

void ZMQSlave::ActionSubscriberLoop(){
    //Run Logger stuff!
    ModelLogger::setup_model_logger(host_name_, logger_endpoint_, true);
    //Construct a filter for this hostname
    //Add a * char so we don't get messages for other similar hosts.
    //gouda1 would match gouda11 but gouda1* doesn't match gouda11*
    std::string name = host_name_ + "*";

    //Construct a Subscriber socket and connect
    auto socket = zmq::socket_t(*context_, ZMQ_SUB);

    std::cout << "Connecting to: " << master_server_address_ << std::endl;
    socket.connect(master_server_address_.c_str());

    //Filter the global messages
    socket.setsockopt(ZMQ_SUBSCRIBE, "*", 1);

    //Filter the specific messages for this node
    socket.setsockopt(ZMQ_SUBSCRIBE, name.c_str(), name.size());

    //Wait for a period of time before recieving
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    zmq::message_t node;
    zmq::message_t action;

    while(true){
        try{
            //Recieve the action's target node first
            socket.recv(&node);
            //Recieve the action second
            socket.recv(&action);

            //Get the messages as strings
            std::string node_str(static_cast<char *>(node.data()), node.size());
            std::string action_str(static_cast<char *>(action.data()), action.size());
           
            std::pair<std::string, std::string> p;
            p.first = node_str;
            p.second = action_str;

            //Lock the Queue, and notify the action queue.
            std::unique_lock<std::mutex> lock(queue_mutex_);
            message_queue_.push(p);
            queue_lock_condition_.notify_all();
        }catch(const zmq::error_t& exception){
            if(exception.num() == ETERM){
                std::cout << "Terminating!" << std::endl;
                //Caught exception
                break;
            }
        }
    }
}

void ZMQSlave::ActionQueueLoop(){
    while(true){
        bool terminated = false;
        std::queue<std::pair<std::string, std::string> > replace_queue;
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait for notify
            queue_lock_condition_.wait(lock);
            
            //Get the termating flag.
            terminated = terminating;
            
            if(!terminated){
                //Swap our queues, only if we aren't meant to terminate
                if(!message_queue_.empty()){
                    message_queue_.swap(replace_queue);
                }
            }
        }

        //Empty our write queue
        while(!replace_queue.empty()){
            auto p = replace_queue.front();
            if(deployment_manager_){
                
                deployment_manager_->ProcessAction(p.first, p.second);
            }
            
            //Convert to Proto
            replace_queue.pop();
        }

        if(terminated){
            //Got a terminate flag from the destructor or a ETERM in the queue_loop
            break;
        }
    }
}
