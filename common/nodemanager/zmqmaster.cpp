#include "zmqmaster.h"
#include <iostream>

ZMQMaster::ZMQMaster(std::string host_name, std::string port, std::vector<std::string> slaves){
    context_ = new zmq::context_t(1);
    port_ = port;
    
    slaves_ = slaves;
    //action_socket_ = new zmq::socket_t(*context_, ZMQ_PUB);
    registration_thread_ = new std::thread(&ZMQMaster::registration_loop, this);
    //
}

ZMQMaster::~ZMQMaster(){

}

bool ZMQMaster::connected_to_slaves(){

    return true;
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

void ZMQMaster::registration_loop(){
    {
        auto socket = zmq::socket_t(*context_, ZMQ_REP);
        //socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

        //Connect registration socket to all slaves addressess.
        for(auto s : slaves_){
            std::cout << "Master Connecting: " << s << std::endl;
            socket.connect(s.c_str()); 
        }
        int slave_count = slaves_.size();
        int count = 0;
        bool got_all_slaves = false;
        zmq::message_t *data = new zmq::message_t();
        zmq::message_t *reply = new zmq::message_t();

        while(true){
            std::cout << "Slaves: " << count << "/" << slave_count << std::endl;
            //Wait for next message
            socket.recv(data);

            //If we have a valid message
            if(data->size() > 0){
                //Construct a string out of the zmq data
                std::string msg_str(static_cast<char *>(data->data()), data->size());
            
                //Check the hostname
                std::cout << "Got Slave: " << msg_str << std::endl;

                zmq::message_t rep(port_.c_str(), port_.size());
                socket.send(rep);

                count++;         
                
                if(count == slave_count){
                    std::cout << " Got Slaves: " << count << std::endl;
                    break;
                }
            }
        }
    }

    std::cout << "Got Clients, Starting Writer Loop!" << std::endl;
    writer_thread_ = new std::thread(&ZMQMaster::writer_loop, this);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    int i = 0;
    while(true){
        //Send happiness
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if(i++ % 5 == 0){
            send_action("*", "FOR ALL");
        }else{
            send_action("client1*", "FOR CLIENT 1");
            send_action("client2*", "FOR CLIENT 2");
        }      
    }
}

void ZMQMaster::writer_loop(){
    auto socket = zmq::socket_t(*context_, ZMQ_PUB);
    socket.bind(port_.c_str());
    while(true){

        std::queue<std::pair<std::string, std::string> > replace_queue;
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait for notify
            queue_lock_condition_.wait(lock);
            //Swap our queues
            if(!message_queue_.empty()){
                message_queue_.swap(replace_queue);
            }
        }

        //Empty our write queue
        while(!replace_queue.empty()){
            auto p = replace_queue.front();
            //std::string str;
            //if(message->SerializeToString(&str)){
            //    zmq::message_t data(str.c_str(), str.size());
            //    return socket_->send(data);
            //}
            std::cout << "Sending to:" << p.first << " ACTION: " << p.second << std::endl;
            zmq::message_t topic(p.first.c_str(), p.first.size());
            zmq::message_t data(p.second.c_str(), p.second.size());
            //Send node name
            socket.send(topic, ZMQ_SNDMORE );
            //Send Data
            socket.send(data);
            replace_queue.pop();
        }
    }
    std::cout << "action_loop thread Finished." << std::endl;
}