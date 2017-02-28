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

std::mutex mutex_;
std::condition_variable lock_condition_;

std::string VERSION_NAME = "LOGAN_SERVER";
std::string VERSION_NUMBER = "1.1";

std::string DEFAULT_ID = "logan_server";
std::string DEFAULT_FILE = "out.sql";

void signal_handler(int sig)
{
	//Gain the lock so we can notify to terminate
	std::unique_lock<std::mutex> lock(mutex_);
	lock_condition_.notify_all();
}

int main(int ac, char** av)
{
	//Connect the SIGINT/SIGTERM signals to our handler.
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	//Variables to store the input parameters
	std::string database_file;	
	std::string server_id;
	std::vector<std::string> client_addresses;

	//Parse command line options
	boost::program_options::options_description desc("Options");
	desc.add_options()("ip,I", boost::program_options::value<std::vector<std::string> >(&client_addresses)->multitoken(), "logan_client endpoints to register against.");
	desc.add_options()("id,i", boost::program_options::value<std::string>(&server_id)->default_value(DEFAULT_ID), "Unique ID for this server.");
	desc.add_options()("out,o", boost::program_options::value<std::string>(&database_file)->default_value(DEFAULT_FILE), "Output file name.");
	desc.add_options()("help,h", "Display help");

	//Construct a variable_map
	boost::program_options::variables_map vm;
	
	try{
		//Parse Argument variables
		boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
		boost::program_options::notify(vm);
	}catch(boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
		std::cout << desc << std::endl;
        return 1;
    }
	
	//Print the help output
	if(vm.count("help")){
		std::cout << desc << std::endl;
		return 0;
	}
	
	//Output Terminal output
	std::cout << "-------[" + VERSION_NAME + " v" + VERSION_NUMBER + "]-------" << std::endl;
	std::cout << "* Output file: " << database_file << std::endl;
	std::cout << "* ID: " << server_id << std::endl;
	std::cout << "* logan_client endpoints: " << std::endl;
	for(auto s : client_addresses){		
		std::cout << "** " << s << std::endl;
	}
	std::cout << "---------------------------------" << std::endl;
	
	//Server is a Registrant. Send our Server ID as the message we are going to register with
	auto registrant = new zmq::Registrant(server_id);

	//Construct a Handler to interface between our ZMQ messaging infrastructure and SQLite
	auto handler = new LogProtoHandler(database_file);

	//Register a callback function to be notified when Server connects to a new Client
	auto fn = std::bind(&LogProtoHandler::ClientConnected, handler, std::placeholders::_1, std::placeholders::_2);
	registrant->RegisterNotify(fn);

	//Connect our Registrant to the Clients we wish to Register with
	for(auto s : client_addresses){
		registrant->AddEndpoint(s);
	}

	//Start the registrant
	registrant->Start();

	//Wait for the signal_handler to notify for exit
	std::unique_lock<std::mutex> lock(mutex_);
	lock_condition_.wait(lock);

	//Free up memory	
	delete registrant;
	delete handler;

    return 0;
}
