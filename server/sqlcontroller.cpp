#include "sqlcontroller.h"
#include <iostream>

SQLController::SQLController(std::vector<std::string> addrs, std::string port, std::string file){

    port_ = port;

    for(auto addr : addrs){
        std::string temp = "tcp://" + addr + ":" + port;
        addresses_.push_back(temp);
    }

    //Setup ZMQ context
    context_ = new zmq::context_t(1);
    
    //Construct our terminate socket
    term_socket_ = new zmq::socket_t(*context_, ZMQ_PUB);
	term_socket_->bind("inproc://term_signal");
    
    //Construct our log database
    log_database_ = new LogDatabase(file);
    
    //Setup our threads
    reciever_thread_ = new std::thread(&SQLController::RecieverThread, this);
    sql_thread_ = new std::thread(&SQLController::SQLThread, this);
}

void SQLController::TerminateReceiver(){
    //Set our terminate
    terminate_reciever_ = true;

    //Send a blank message
    term_socket_->send(zmq::message_t());

    //Wait for the reciever_thread_ to finish
    reciever_thread_->join();

    delete term_socket_;
    delete context_;
}

SQLController::~SQLController(){
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        terminate_sql_ = true;
        queue_lock_condition_.notify_all();
    }

    //The reciever_thread_ should have finished (terminate_reciever is called prior to sql controller)
    delete reciever_thread_;

    //Wait for the sql_thread to terminate
    sql_thread_->join();
    delete sql_thread_;
    
    //Teardown database
    delete log_database_;
}

void SQLController::RecieverThread(){
    //Setup our Subscriber socket
    zmq::socket_t socket(*context_, ZMQ_SUB);
	
    //Subscribe to everything
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    //Connect to our terminate inprocess socket
    socket.connect("inproc://term_signal");

    //Connect to all nodes on our network
	for (auto a : addresses_){
		socket.connect(a.c_str());
	}
	
    zmq::message_t *data = new zmq::message_t();
    
    while(!terminate_reciever_){
		try{
            //Wait for next message
			socket.recv(data);

            //If we have a valid message
            if(data->size() > 0){
                //Construct a string out of the zmq data
                std::string msg_str(static_cast<char *>(data->data()), data->size());
             
                //Gain the lock so we can push this message onto our queue
                std::unique_lock<std::mutex> lock(queue_mutex_);
                rx_message_queue_.push(msg_str);

                //Notify the condition when we have > 20 messages to process
                if(rx_message_queue_.size() > 20){
                    queue_lock_condition_.notify_all();
                }
            }
        }catch(zmq::error_t ex){
            //Do nothing with an error.
			continue;
        }
    }
    //Free our memory
    delete data;
}


void SQLController::SQLThread(){
    //Update loop.
    while(!terminate_sql_){
        std::queue<std::string> replace_queue;
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait for the condition to be notified
            queue_lock_condition_.wait(lock);
            
            //Now that we have access, swap out queues and release the mutex
            if(!rx_message_queue_.empty()){
                rx_message_queue_.swap(replace_queue);
            }
        }

        //Construct a Protobuf object
        SystemStatus* message = new SystemStatus();

        //Empty the queue
        while(!replace_queue.empty()){
            //Fill our pb status message with the contents from the string
            if (message->ParseFromString(replace_queue.front())){
                //dump to the contents to sql statements
                log_database_->process_status(message);
            }else{
                std::cout << "Cannot process message: " <<  replace_queue.front() << std::endl;
            }
            
            replace_queue.pop();
            
            //clear our message
            message->Clear();
        }
        //Free the memory
        delete message;
    }
}