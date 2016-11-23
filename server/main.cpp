#include <iostream>
#include <signal.h>

#include "zmq.hpp"
#include "systemstatus.pb.h"

#include "logdatabase.h"

static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

int main()
{
	s_catch_signals();
    SystemStatus message;

    //Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_SUB);
	
	//Subscribe to everything
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    //Multicast listen.
	socket.bind("tcp://*:5555");
	
	/*
	int port = 5555;
	std::string address_str("tcp://192.168.111.");
	int address;
	for (address = 1; address < 255; address++){
		std::string connect_addr = address_str + std::to_string(address) + ":" + std::to_string(port);
		socket.connect(connect_addr.c_str());
	}*/
	
	
    int count = 0;

	//Construct a new log database.
	//TODO: take commandline option
	LogDatabase* db = new LogDatabase("test.sql");

	zmq::message_t *data = new zmq::message_t();
    while(!s_interrupted){
		//Recieve the next data element

		try{
			socket.recv(data);
			std::string msg_str(static_cast<char *>(data->data()), data->size());

			if (message.ParseFromString(msg_str)){
				db->process_status(&message);
			}else{
				std::cout << data->size() << std::endl;
			}
		}
		catch(zmq::error_t ex){
			continue;
		}

		
    }


	delete db;
}
