#include <signal.h>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <vector>
#include <boost/program_options.hpp>

#include "logprotohandler.h"

std::mutex mutex_;
std::condition_variable lock_condition_;

std::string VERSION_NAME = "LOGAN_SERVER";
std::string VERSION_NUMBER = "1.1";
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
	std::string database_path;	
	std::vector<std::string> client_addresses;

	//Parse command line options
	boost::program_options::options_description desc("Options");
	desc.add_options()("clients,c", boost::program_options::value<std::vector<std::string> >(&client_addresses)->multitoken()->required(), "logan_client endpoints to register against (ie tcp://192.168.1.1:5555)");
	desc.add_options()("database,d", boost::program_options::value<std::string>(&database_path)->default_value(DEFAULT_FILE), "Output SQLite Database file path.");
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
	
	//Print output
	std::cout << "-------[" + VERSION_NAME +" v" + VERSION_NUMBER + "]-------" << std::endl;
	std::cout << "* Database: " << database_path << std::endl;
	for(int i = 0; i < client_addresses.size(); i++){
		if(i == 0){
			std::cout << "* Clients:" << std::endl;
		}
		std::cout <<"** " << client_addresses[i] << std::endl;
	}
	std::cout << "---------------------------------" << std::endl;

	//Construct a Handler to interface between our ZMQ messaging infrastructure and SQLite
	auto handler = new LogProtoHandler(database_path, client_addresses);

	//Wait for the signal_handler to notify for exit
	std::unique_lock<std::mutex> lock(mutex_);
	lock_condition_.wait(lock);

	//Free up memory	
	delete handler;
    return 0;
}
