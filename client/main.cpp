#include <chrono>
#include <iostream>
#include <signal.h>
#include <thread>
#include "boost/program_options.hpp"

#include "sigarsysteminfo.h"
#include "logcontroller.h"

std::condition_variable lock_condition_;
std::mutex mutex_;
std::string VERSION_NAME = "LOGAN_CLIENT";
std::string VERSION_NUMBER = "1.0";

void signal_handler (int signal_value){
	//Gain the lock so we can notify to terminate
	std::unique_lock<std::mutex> lock(mutex_);
	lock_condition_.notify_all();
}


int main(int ac, char** av){

	//Handle the SIGINT/SIGTERM signal
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	//Parse command line options
	int port;
	bool cached;
    std::vector<std::string> processes;
	double frequency;

	boost::program_options::options_description desc("Options");
	desc.add_options()("port,p",boost::program_options::value<int>(&port)->default_value(5555), "Port number");
	desc.add_options()("cached,c", "Cached mode");
	desc.add_options()("frequency,f", boost::program_options::value<double>(&frequency)->default_value(10.0), "Recording frequency");
	desc.add_options()("process,P", boost::program_options::value<std::vector<std::string> >(&processes)->multitoken(), "Interested processes");
	desc.add_options()("help,h", "Display help");

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
	boost::program_options::notify(vm);

	if(vm.count("help")){
		std::cout << desc << std::endl;
		return 0;
	}

	cached = vm.count("cached");

	std::cout << "-------[" + VERSION_NAME +" v" + VERSION_NUMBER + "]-------" << std::endl;
	std::cout << "* ZMQ Port: " << port << std::endl;
	std::cout << "* Frequency: " << frequency << std::endl;

	if(cached){
		std::cout << "* Live Logging: Off" << std::endl;
	} else {
		std::cout << "* Live Logging: On" << std::endl;
	}

	std::cout << "---------------------------------" << std::endl;


	//Initialise log controller
    LogController* log_controller = new LogController(port, frequency, processes, cached);
	std::cout << "# Starting Logging." << std::endl;

	{
		std::unique_lock<std::mutex> lock(mutex_);
		//Wait for the signal_handler to notify for exit
		lock_condition_.wait(lock);
	}

    //Blocking terminate call.
    //Will wait for logging and writing threads to complete
    log_controller->Terminate();

    delete log_controller;
    return 0;
}
