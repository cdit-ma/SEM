#include <condition_variable>
#include <mutex>
#include <signal.h>
#include <iostream>
#include "boost/program_options.hpp"

#include "sqlcontroller.h"

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

	//TODO: take command line args

	//Parse command line options
	std::string port;
	std::string ip_addr;
	std::string file_name;

	boost::program_options::options_description desc("Options");
	desc.add_options()("ip,i",boost::program_options::value<std::string>(&ip_addr)->default_value(DEFAULT_IP), "ip addr");
	desc.add_options()("port,p",boost::program_options::value<std::string>(&port)->default_value(DEFAULT_PORT), "Port number");
	desc.add_options()("out-file,o",boost::program_options::value<std::string>(&file_name)->default_value(DEFAULT_FILE), "Output file name");
	desc.add_options()("help,h", "Display help");

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
	boost::program_options::notify(vm);

	if(vm.count("help")){
		std::cout << desc << std::endl;
		return 0;
	}

	std::cout << "-------[" + VERSION_NAME +" v" + VERSION_NUMBER + "]-------" << std::endl;
	std::cout << "* Listening on: " << ip_addr << "*:" << port << std::endl;
	std::cout << "* Output file: " << file_name << std::endl;
	std::cout << "---------------------------------" << std::endl;

	SQLController* sql_controller = new SQLController(ip_addr, port, file_name);

	{ 			
		std::unique_lock<std::mutex> lock(mutex_);
		//Wait for the signal_handler to notify for exit
		lock_condition_.wait(lock);
	}

	//Terminate the reciever and reciever thread
	sql_controller->TerminateReceiver();

    //Teardown the SQL Controller which will write the remaining queued messages
	delete sql_controller;
    return 0;
}
