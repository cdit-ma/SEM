#include "sqlcontroller.h"
#include "zmq.hpp"
#include <iostream>

SQLController::SQLController(zmq::context_t *context){
    
    context_ = context;
    terminate_ = false;
    log_database = new LogDatabase("test.sql");
    sql_thread_ = new std::thread(&SQLController::SQLThread, this);
}

SQLController::~SQLController(){
    std::cout << "1" << std::endl;
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        terminate_ = true;
        queue_lock_condition_.notify_all();
    }

    std::cout << "3" << std::endl;
    //Process messages
    sql_thread_->join();
    delete sql_thread_;

    std::cout << "4" << std::endl;

    //Teardown DB
    delete log_database;
    std::cout << "5" << std::endl;
}

void SQLController::RecieverThread(){
    
    
    zmq::socket_t socket(*context_, ZMQ_SUB);
	
	//Subscribe to everything
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    socket.bind("inproc://term_signal");

    int port = 5555;
	std::string address_str("tcp://192.168.111.");
	int address;
	for (address = 1; address < 255; address++){
		std::string connect_addr = address_str + std::to_string(address) + ":" + std::to_string(port);
		socket.connect(connect_addr.c_str());
	}
	
    int count = 0;

    zmq::message_t *data = new zmq::message_t();
    while(!terminate_){
		//Recieve the next data element

		try{
			socket.recv(data);
            std::cout << data->size() << std::endl;
            if(data->size() == 0){
                break;
            }
			std::string msg_str(static_cast<char *>(data->data()), data->size());
            {
                //Gain the lock so we can notify and set our terminate flag.
                std::unique_lock<std::mutex> lock(queue_mutex_);
                rx_message_queue_.push(msg_str);

                if(rx_message_queue_.size() > 20){
                    queue_lock_condition_.notify_all();
                }
            }
		}
		catch(zmq::error_t ex){
			continue;
        }
    }
    delete data;
}


void SQLController::SQLThread(){
    

    //Update loop.
    while(!terminate_){
        std::queue<std::string> replace_queue;
        {
            //Lock the Queue, and notify the writer queue.
             //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait for notify
            queue_lock_condition_.wait(lock);
            //Swap our queues
            if(!rx_message_queue_.empty()){
                rx_message_queue_.swap(replace_queue);
            }
        }

        
        //Empty our write queue
        while(!replace_queue.empty()){
            SystemStatus* message = new SystemStatus();
            if (message->ParseFromString(replace_queue.front())){
                log_database->process_status(message);
            }
            replace_queue.pop();
        }
    }
}