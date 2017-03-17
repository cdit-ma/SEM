#include <iostream>
#include "cpuworker.h"
#include <chrono>
#include <thread>
#include <core/component.h>

int main(){

    std::cout << "TESTY TEST" << std::endl;
    Component* c = new Component("Tets");

    auto cpu = new CpuWorker(c, "WE_CPU");
    cpu->IntOp(100.0);
    return 0;
}
