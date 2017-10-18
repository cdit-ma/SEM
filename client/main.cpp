/* logan
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "logan"
 *
 * "logan" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "logan" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#include <signal.h>
#include <condition_variable>
#include <mutex>
#include <string>
#include <functional>
#include <boost/program_options.hpp>
#include <iostream>

#include "logcontroller.h"

std::condition_variable lock_condition_;
std::mutex mutex_;
std::string VERSION_NAME = "LOGAN_CLIENT";
std::string VERSION_NUMBER = "1.3.1";

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
	bool live_mode = false;
    std::vector<std::string> processes;
	double log_frequency = 1.0;
	std::string publisher_endpoint;

	boost::program_options::options_description desc("Options");
	desc.add_options()("publisher,p", boost::program_options::value<std::string>(&publisher_endpoint), "ZMQ Publisher endpoint (ie tcp://192.168.1.1:5555)");
	desc.add_options()("frequency,f", boost::program_options::value<double>(&log_frequency)->default_value(log_frequency), "Logging frequency (Hz)");
	desc.add_options()("process,P", boost::program_options::value<std::vector<std::string> >(&processes)->multitoken(), "Process names to log (ie logan_client)");
	desc.add_options()("live_mode,l", boost::program_options::value<bool>(&live_mode)->default_value(live_mode), "Produce data live");
	desc.add_options()("system_info_print,s", "Print system info then exit");
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

	if(!vm.count("system_info_print")){
		if(publisher_endpoint.empty()){
			std::cout << "No publisher endpoint supplied, terminating." << std::endl;
			return 0;
		}
		//Print output
		std::cout << "-------[" + VERSION_NAME +" v" + VERSION_NUMBER + "]-------" << std::endl;
		std::cout << "* Publisher ZMQ endpoint: " << publisher_endpoint << std::endl;
		std::cout << "* Frequency: " << log_frequency << std::endl;
		std::cout << "* Live Logging: " << (live_mode ? "On" : "Off") << std::endl;
		for(int i = 0; i < processes.size(); i++){
			if(i == 0){
				std::cout << "* Monitoring Processes:" << std::endl;
			}
			std::cout << "** " << processes[i] << std::endl;
		}
		std::cout << "---------------------------------" << std::endl;
		//Initialise log controller
		LogController* log_controller = new LogController(publisher_endpoint, log_frequency, processes, live_mode);
		
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
	}
	else{
		LogController* log_controller = new LogController();
		log_controller->Print();
		delete log_controller;
	}


    return 0;
}
