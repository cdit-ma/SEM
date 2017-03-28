#include <iostream>
#include "cpu_worker.h"
#include <chrono>

int main(){
    std::cout << "CPU worker tester" << std::endl;
    
    Cpu_Worker worker(0, "test");

    double run_count = 1000000000;

    std::cout << "Running " << run_count << " int operations. This should take a measurable amount of time." << std::endl;
    auto start = std::chrono::steady_clock::now();
    
    int result = worker.IntOp(run_count);
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);


    if(result != 0){
        std::cout << "IntOp failed, return code: " << result << std::endl;
    }
    else if(ms.count() < 50){
        std::cout << "IntOp failed, operation completed too quickly." << std::endl;
    }
    else{
        std::cout << "Done IntOp in " << ms.count() << " milliseconds" << std::endl;
    }


    std::cout << "Running " << run_count << " float operations. This should take a measurable amount of time." << std::endl;
    start = std::chrono::steady_clock::now();
    
    result = worker.FloatOp(run_count);
    end = std::chrono::steady_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);


    if(result != 0){
        std::cout << "FloatOp failed, return code: " << result << std::endl;
    }
    else if(ms.count() < 50){
        std::cout << "FloatOp failed, operation completed too quickly." << std::endl;
    }
    else{
        std::cout << "Done FloatOp in " << ms.count() << " milliseconds" << std::endl;
    }
    return 0;
}