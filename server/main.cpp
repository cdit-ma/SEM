#include <iostream>
#include <signal.h>

#include "zmq.hpp"
#include "sqlcontroller.h"

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
	s_catch_signals();

    SQLController*  sqlController = new SQLController();


    while(!s_interrupted){
	    std::this_thread::sleep_for(std::chrono::seconds(1));
    }


	std::cout << "YOO YOYOYO" << std::endl;
    delete sqlController;
	std::cout << "YOO YOYOYO" << std::endl;
    return 0;
}
