#include <iostream>
#include <signal.h>
#include <thread>

#include "zmq.hpp"
#include "sqlcontroller.h"

zmq::socket_t* term_socket = 0;

void terminate_zmq(){
	if(term_socket){
		//Send a blank message to terminate
		zmq::message_t data("",0);
		term_socket->send(data);
	}
}

void signal_handler (int signal_value)
{
	terminate_zmq();
}

int main()
{
	//Handle the interupt signal
	signal(SIGINT, signal_handler);

	//Construct ZMQ context
	zmq::context_t* context = new zmq::context_t(1);
	//Construct a terminate socket
	term_socket = new zmq::socket_t(*context, ZMQ_PUB);
	term_socket->connect("inproc://term_signal");
	
	SQLController* sql_controller = new SQLController(context);
    std::thread* reciever_thread_ = new std::thread(&SQLController::RecieverThread, sql_controller);
	//Wait for the reciever_thread to finish
	reciever_thread_->join();
	delete reciever_thread_;
	delete sql_controller;
    return 0;
}
