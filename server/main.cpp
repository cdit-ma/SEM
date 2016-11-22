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
    SystemStatus message;

    //Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_SUB);
	
	//Subscribe to everything
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    //Multicast listen.
	//socket.bind("tcp://*:5555");
	//TODO: Connect based on command line args or input file
    socket.connect("tcp://192.168.111.187:5555");
    socket.connect("tcp://192.168.111.246:5555");
	socket.connect("tcp://192.168.111.247:5555");
	socket.connect("tcp://192.168.111.80:5555");
	socket.connect("tcp://192.168.111.81:5555");
	
    int count = 0;

	//Construct a new log database.
	//TODO: take commandline option
	LogDatabase* db = new LogDatabase("test.sql");

	zmq::message_t *data = new zmq::message_t();
    while(!s_interrupted){
		//Recieve the next data element
		socket.recv(data);

		std::string msg_str(static_cast<char *>(data->data()), data->size());

		if (message.ParseFromString(msg_str)){
			//std::cout << message.ByteSize() << std::endl;
			db->process_status(&message);
		}else{
			std::cout << data->size() << std::endl;
		}
    }

	delete db;
}
