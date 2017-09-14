#include <iostream>
#include "cpu_worker.h"
#include <chrono>
#include <limits>


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

    std::cout << "Running trivial matrix mult. 2*2 matrix, non zeroed output matrix." << std::endl;

    std::vector<float> trivialMatA(2*2);
    std::vector<float> trivialMatB(2*2);
    std::vector<float> trivialMatC(2*2);

    std::vector<float> expected {2, 3, 6, 11};

    for (unsigned int index=0; index<4; index++) trivialMatA[index] = (float)index;
	for (unsigned int index=0; index<4; index++) trivialMatB[index] = (float)index;
    for (unsigned int index=0; index<4; index++) trivialMatC[index] = (float)index;
    
    result = worker.MatrixMult(trivialMatA, trivialMatB, trivialMatC);

    if(result != 0){
        std::cout << "Trivial MatrixMult failed, return code: " << result << std::endl;
    }else{
        if(trivialMatC == expected){
            std::cout << "Success" << std::endl;
        }else{
            std::cout << "Failure" << std::endl;
        }
    }

    
    std::vector<float> matA(1024*1024);
    std::vector<float> matB(1024*1024);
    std::vector<float> matC(1024*1024);

    for (unsigned int index=0; index<1024*1024; index++) matA[index]=(float)index;
	for (unsigned int index=0; index<1024*1024; index++) matB[index]=(float)index;
	for (unsigned int index=0; index<1024*1024; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

    std::cout << "Running matrix mult on size 1024*1024 matrix. This should take a between 5 and 10 seconds on a modern cpu." << std::endl;

    start = std::chrono::steady_clock::now();

    result = worker.MatrixMult(matA, matB, matC);

    end = std::chrono::steady_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if(result != 0){
        std::cout << "MatrixMult failed, return code: " << result << std::endl;
    }
    else if(ms.count() < 1000){
        std::cout << "MatrixMult failed, operation completed too quickly." << std::endl;
    }
    else{
        std::cout << "Done MatrixMult in " << ms.count() << " milliseconds" << std::endl;
    }


    return 0;
}

