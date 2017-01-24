#include "zmqmaster.h"
#include <iostream>

template<typename T>
inline void remove(std::vector<T> & v, const T & item)
{
    v.erase(std::remove(v.begin(), v.end(), item), v.end());
}

ZMQMaster::ZMQMaster(std::string host_name, std::string endpoint, std::string graphml_path){
    context_ = new zmq::context_t(1);
    endpoint_ = endpoint;

    execution_manager_ = new ExecutionManager(this, graphml_path);

    slaves_ = execution_manager_->get_slave_endpoints();

    //Start the registration thread
    registration_thread_ = new std::thread(&ZMQMaster::registration_loop, this);
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

    if(registration_thread_){
        std::cout << "Deleting registration_thread_" << std::endl;
        registration_thread_->join();
        delete registration_thread_;
    }

    if(writer_thread_){
        std::cout << "Deleting writer_thread_" << std::endl;
        writer_thread_->join();
        delete writer_thread_;
    }
}

bool ZMQMaster::action_writer_active(){
    return writer_active;
}

void ZMQMaster::send_action(std::string node_name, std::string action){
    std::pair<std::string, std::string> p;
    p.first = node_name;
    p.second = action;
    
    //Lock the Queue, and notify the writer queue.
    std::unique_lock<std::mutex> lock(queue_mutex_);
    message_queue_.push(p);
    queue_lock_condition_.notify_all();
}

void ZMQMaster::send_action(std::string node_name, google::protobuf::MessageLite* message){
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

void ZMQMaster::registration_loop(){
    auto socket = zmq::socket_t(*context_, ZMQ_REP);
    
    auto unregistered_slaves = slaves_;
    //Connect registration socket to all slaves addressess.
    for(auto s : unregistered_slaves){
        std::cout << "Master Connecting to Slave: " << s << std::endl;
        socket.connect(s.c_str()); 
    }

    //Wait for a period of time before recieving
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    zmq::message_t slave_addr;
    

    while(!unregistered_slaves.empty()){
        try{
            //Wait for Slave to send a message
            socket.recv(&slave_addr);


            //Construct a string out of the zmq data
            std::string slave_addr_str(static_cast<char *>(slave_addr.data()), slave_addr.size());
            std::string host_name = execution_manager_->get_host_name_from_address(slave_addr_str);

            //Remove the slave which has just registered from the vector of unregistered slaves
            remove(unregistered_slaves, slave_addr_str);
            
            zmq::message_t server_addr(endpoint_.c_str(), endpoint_.size());
            zmq::message_t slave_hostname(host_name.c_str(), host_name.size());
            //Send the server address for the publisher
            socket.send(server_addr, ZMQ_SNDMORE);
            //Send the slave hostname
            socket.send(slave_hostname);
        }catch(const zmq::error_t& exception){
            if(exception.num() == ETERM){
                std::cout << "Terminating Registration Thread!" << std::endl;
            }
            return;
        }
    }

    //Start up the writer thread
    writer_thread_ = new std::thread(&ZMQMaster::writer_loop, this);

    //Wait for 
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::cout << "SENDING EXECUTION INSTRUCTIONS" << std::endl;
    execution_manager_->execution_loop();
    std::cout << "SENT EXECUTION INSTRUCTIONS" << std::endl;
}

void ZMQMaster::writer_loop(){
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