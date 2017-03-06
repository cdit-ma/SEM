#include <iostream>
#include "WE_CPU.h"
#include <chrono>
#include <thread>

int main(){

    std::cout << "TESTY TEST" << std::endl;

    auto cpu = new WE_CPU();

    std::thread threada(&WE_CPU::IntOp, cpu, 36000000000000);

    threada.join();

    return 0;
}
