#include "zmqmaster.h"
#include <iostream>

template<typename T>
inline void remove(std::vector<T> & v, const T & item)
{
    v.erase(std::remove(v.begin(), v.end(), item), v.end());
}

ZMQMaster::ZMQMaster(std::string endpoint, std::string graphml_path){
    context_ = new zmq::context_t(1);

    endpoint_ = endpoint;
    execution_manager_ = new ExecutionManager(this, graphml_path);
    slaves_ = execution_manager_->GetSlaveEndpoints();


    for(auto s : slaves_){
        auto t = new std::thread(&ZMQMaster::RegistrationLoop, this, s);
    }
}

ZMQMaster::~ZMQMaster(){
    {
        //Unblock our mutex condition guarding
        std::unique_lock<std::mutex> lock(queue_mutex_);
        terminating = true;
        queue_lock_condition_.notify_all();
    }

    //Deleting the context will interupt any blocking ZMQ calls
    if(context_){
        std::cout << "Deleting Context" << std::endl;
        delete context_;    
    }

    while(!registration_threads_.empty()){
        auto t = registration_threads_.back();
        if(t){
            t->join();
            delete t;
        }
        registration_threads_.pop_back();
    }

    if(writer_thread_){
        std::cout << "Deleting writer_thread_" << std::endl;
        writer_thread_->join();
        delete writer_thread_;
    }
}

bool ZMQMaster::ActionWriterActivate(){
    return writer_active;
}

void ZMQMaster::SendAction(std::string node_name, std::string action){
    std::pair<std::string, std::string> p;
    p.first = node_name;
    p.second = action;
    
    //Lock the Queue, and notify the writer queue.
    std::unique_lock<std::mutex> lock(queue_mutex_);
    message_queue_.push(p);
    queue_lock_condition_.notify_all();
}

void ZMQMaster::SendAction(std::string node_name, google::protobuf::MessageLite* message){
    std::string action;

    if(!message->SerializeToString(&action)){
        std::cout << "Serialization failed!" << std::endl;
        return;
    }

    std::pair<std::string, std::string> p;
    p.first = node_name;
    p.second = action;
    
    //Lock the Queue, and notify the writer queue.
    std::unique_lock<std::mutex> lock(queue_mutex_);
    message_queue_.push(p);
    queue_lock_condition_.notify_all();
}

void ZMQMaster::RegistrationLoop(std::string endpoint){
    //Construct a socket (Using Pair)
    auto socket = zmq::socket_t(*context_, ZMQ_PAIR);

    try{
        //Connect to the socket
        socket.connect(endpoint.c_str()); 
    }catch(zmq::error_t &ex){
        std::cout << "ZMQMaster::RegistrationLoop():Connect(" << endpoint << "): " << ex.what() << std::endl;
    }

    //Messages from the slave
    zmq::message_t slave_addr;
    zmq::message_t slave_response;

    while(true){
        try{
            //Wait for Slave to send its endpoint
            socket.recv(&slave_addr);

            std::string slave_addr_str(static_cast<char *>(slave_addr.data()), slave_addr.size());
            
            //Get the matching hostname from the execution manager
            std::string host_name = execution_manager_->GetHostNameFromAddress(slave_addr_str);
            std::string slave_logger_pub_addr_str = "";

            //Construct our reply messages
            zmq::message_t master_control_pub_addr(endpoint_.c_str(), endpoint_.size());
            zmq::message_t slave_name(host_name.c_str(), host_name.size());
            zmq::message_t slave_logging_pub_addr(slave_logger_pub_addr_str.c_str(), slave_logger_pub_addr_str.size());
            
            //Send the server address for the publisher
            socket.send(master_control_pub_addr, ZMQ_SNDMORE);
            //Send the slave hostname
            socket.send(slave_name, ZMQ_SNDMORE);
            //Send the slave hostname
            socket.send(slave_logging_pub_addr);

            //Wait for Slave to send a message
            socket.recv(&slave_response);
            std::string slave_response_str(static_cast<char *>(slave_response.data()), slave_response.size());
        }catch(const zmq::error_t& exception){
            if(exception.num() != ETERM){
                std::cout << "ZMQMaster::RegistrationLoop(): " << exception.what() << std::endl;
            }
            break;
        }
    }
}

void ZMQMaster::WriterLoop(){
    auto socket = zmq::socket_t(*context_, ZMQ_PUB);
    socket.bind(endpoint_.c_str());

    //Wait for a period of time before trying to send
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    writer_active = true;

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

            zmq::message_t topic(p.first.c_str(), p.first.size());
            zmq::message_t data(p.second.c_str(), p.second.size());
            try{
                //Send node name
                socket.send(topic, ZMQ_SNDMORE);
                //Send Data
                socket.send(data);
            }catch(const zmq::error_t& exception){
                if(exception.num() == ETERM){
                    std::cout << "Terminating Writer Thread!" << std::endl;
                }
                terminated = true;
                break;
            }
            replace_queue.pop();
        }

        if(terminated){
            //Got a terminate flag from the destructor or a ETERM in the queue_loop
            break;
        }
    }
    std::cout << "action_loop thread Finished." << std::endl;
}