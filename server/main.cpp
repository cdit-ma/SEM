#include <signal.h>
#include <mutex>
#include <condition_variable>
#include <iostream>

#include "sqlcontroller.h"

std::condition_variable lock_condition_;
std::mutex mutex_;

void signal_handler (int signal_value)
{
	std::cout << "SIGNAL: " << signal_value << std::endl;
	//Gain the lock so we can notify to terminate
	std::unique_lock<std::mutex> lock(mutex_);
	lock_condition_.notify_all();
}

int main()
{
	//Handle the interupt signal
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	SQLController* sql_controller = new SQLController();

	{ 			
		std::unique_lock<std::mutex> lock(mutex_);
		//Wait for the signal_handler to notify for exit
		lock_condition_.wait(lock);
	}

	//Terminate the reciever and reciever thread
	sql_controller->terminate_reciever();

    //Teardown the SQL Controller which will write the remaining queued messages
	delete sql_controller;
    return 0;
}
