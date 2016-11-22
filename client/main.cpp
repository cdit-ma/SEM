#include "sigarsysteminfo.h"

#include <iostream> //stdcout
#include <thread>
#include <chrono>

#include "logcontroller.h"

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

//g++ main.cpp sigarsysteminfo.cpp -lsigar -I/Users/dan/Desktop/sigar/include/
int main(int, char**){

    s_catch_signals();

    //TODO: take command line options
    std::vector<std::string> processes;
    processes.push_back("logan_client");
    LogController* logController = new LogController(1, processes, false);

    //TODO: terminate correctly

    while(true){
	std::this_thread::sleep_for(std::chrono::seconds(15));
    }

    return 0;
}