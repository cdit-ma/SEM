#include "sigarsysteminfo.h"

#include <iostream> //stdcout
#include <thread>
#include <chrono>

#include "logcontroller.h"



//g++ main.cpp sigarsysteminfo.cpp -lsigar -I/Users/dan/Desktop/sigar/include/
int main(int, char**){
    LogController* logController = new LogController();

    while(true){
	std::this_thread::sleep_for(std::chrono::seconds(15));
    }
    

    return 0;
}