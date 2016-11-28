#include <chrono>
#include <iostream>
#include <signal.h>
#include <thread>

#include "sigarsysteminfo.h"
#include "logcontroller.h"

std::condition_variable lock_condition_;
std::mutex mutex_;

void signal_handler (int signal_value){
	//Gain the lock so we can notify to terminate
	std::unique_lock<std::mutex> lock(mutex_);
	lock_condition_.notify_all();
}

int main(int, char**){

	//Handle the SIGINT/SIGTERM signal
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

    //TODO: take command line options
    std::vector<std::string> processes;
    processes.push_back("logan_client");
    processes.push_back("logan_server");

    LogController* log_controller = new LogController(10, processes, false);

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
