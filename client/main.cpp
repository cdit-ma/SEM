#include <signal.h>
#include <condition_variable>
#include <mutex>
#include <string>
#include <functional>
#include <boost/program_options.hpp>
#include <iostream>

#include "../re_common/zmq/registrar/registrar.h"

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
	int port = 5555;
	bool cached = false;
    std::vector<std::string> processes;
	double frequency = 1.0;
	std::string ip_addr;

	boost::program_options::options_description desc("Options");
	desc.add_options()("port,p",boost::program_options::value<int>(&port)->default_value(port), "Port number");
	desc.add_options()("register,r", boost::program_options::value<std::string>(&ip_addr)->default_value(""), "IP REGISTRAR number");
	desc.add_options()("cached,c", "Cached mode");
	desc.add_options()("frequency,f", boost::program_options::value<double>(&frequency)->default_value(frequency), "Recording frequency");
	desc.add_options()("process,P", boost::program_options::value<std::vector<std::string> >(&processes)->multitoken(), "Interested processes");
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

	cached = vm.count("cached") > 0;

	std::cout << "-------[" + VERSION_NAME +" v" + VERSION_NUMBER + "]-------" << std::endl;
	std::cout << "* ZMQ Port: " << port << std::endl;
	std::cout << "* Frequency: " << frequency << std::endl;

	if(cached){
		std::cout << "* Live Logging: Off" << std::endl;
	} else {
		std::cout << "* Live Logging: On" << std::endl;
	}

	std::cout << "* Monitoring Processes:" << std::endl;

	for(auto s : processes){
		std::cout <<"\t** " << s << std::endl;
	}

	
	std::cout << "---------------------------------" << std::endl;

	std::string r_port_string = "tcp://*:" + std::to_string(port+1);
	std::cout << "Registrar PORT: " << ip_addr << std::endl;
	//LOGAN CLIENT = REGISTRAR
	auto registrar = new zmq::Registrar(ip_addr, "TEST HLELLO");
	

	


	//Initialise log 	troller
    LogController* log_controller = new LogController(port, frequency, processes, cached);

	auto  fn = std::bind(&LogController::GotNewServer, log_controller, std::placeholders::_1);
	registrar->RegisterNotify(fn);

	registrar->Start();
	std::cout << "# Starting Logging." << std::endl;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		//Wait for the signal_handler to notify for exit
		lock_condition_.wait(lock);
	}

	delete registrar;

    //Blocking terminate call.
    //Will wait for logging and writing threads to complete
    log_controller->Terminate();

    delete log_controller;
	
    return 0;
}
