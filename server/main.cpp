#include <condition_variable>
#include <mutex>
#include <signal.h>
#include <functional>
#include <iostream>
#include <boost/program_options.hpp>

#include "../re_common/zmq/protoreceiver/zmqreceiver.h"
#include "../re_common/zmq/registrant/registrant.h"
#include "logprotohandler.h"
#include "sqlitedatabase.h"

#include <vector>
std::condition_variable lock_condition_;
std::mutex mutex_;

std::string VERSION_NAME = "LOGAN_SERVER";
std::string VERSION_NUMBER = "1.0";

std::string DEFAULT_IP = "tcp://192.168.111.";
std::string DEFAULT_PORT = "5555";
std::string DEFAULT_FILE = "out.sql";

void signal_handler (int signal_value)
{
	//Gain the lock so we can notify to terminate
	std::unique_lock<std::mutex> lock(mutex_);
	lock_condition_.notify_all();
}


int main(int ac, char** av)
{
	//Handle the SIGINT/SIGTERM signal
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	std::string port;
	std::string ip_addr;
	
	std::string file_name;
	std::vector<std::string> addresses;

	//Parse command line options
	boost::program_options::options_description desc("Options");
	desc.add_options()("ip,I", boost::program_options::value<std::vector<std::string> >(&addresses)->multitoken(), "IP addresses to connect to");
	desc.add_options()("register,r", boost::program_options::value<std::string>(&ip_addr)->default_value(DEFAULT_PORT), "IP REGISTRAR number");
	desc.add_options()("port,p", boost::program_options::value<std::string>(&port)->default_value(DEFAULT_PORT), "Port number");
	desc.add_options()("out-file,o", boost::program_options::value<std::string>(&file_name)->default_value(DEFAULT_FILE), "Output file name");
	desc.add_options()("help,h", "Display help");

	boost::program_options::variables_map vm;
	
	//Catch invalid options
	try{
		boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
		boost::program_options::notify(vm);
	}catch(boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
		std::cout << desc << std::endl;
        return 1;
    }

	if(vm.count("help")){
		std::cout << desc << std::endl;
		return 0;
	}

	std::cout << "-------[" + VERSION_NAME + " v" + VERSION_NUMBER + "]-------" << std::endl;
	std::cout << "* Output file: " << file_name << std::endl;
	std::cout << "* Listening on port: " << port << std::endl;
	std::cout << "* Listening to: " << std::endl;
	for(auto s : addresses){		
		
		std::cout << "** " << s << std::endl;
	}
	std::cout << "---------------------------------" << std::endl;
	
	std::cout << "CONNECTING TO : " << ip_addr << std::endl;
	
	//LOGAN SERVER = Registrant (one who registers)
	//START A REGISTRANT
	auto registrant = new zmq::Registrant(ip_addr);
	
	ZMQReceiver* receiver = new ZMQReceiver();
	receiver->Start();

	SQLiteDatabase* sql_database = new SQLiteDatabase(file_name);
	LogProtoHandler* proto_handler = new LogProtoHandler(receiver, sql_database);

	//Register a callback into into the proto hanler class to register a new client
	auto callback = [proto_handler, receiver](std::string endpoint, std::string topic) {proto_handler->ClientConnected(endpoint);receiver->Connect(endpoint); std::cout << topic << std::endl;};
	registrant->RegisterNotify(callback);

	//Connect to our senders
	for(auto s : addresses){
		registrant->AddEndpoint(s);
	}
	
	registrant->Start();
	{
		std::unique_lock<std::mutex> lock(mutex_);
		//Wait for the signal_handler to notify for exit
		lock_condition_.wait(lock);
	}

	delete registrant;

	//Terminate the reciever and reciever thread
	receiver->TerminateReceiver();

    //Teardown the SQL Controller which will write the remaining queued messages
	delete receiver;
	delete sql_database;
	delete proto_handler;
    return 0;
}
