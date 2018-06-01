#include "../cpu_worker_impl.h"
#include <chrono>
#include <iostream>
#include <string>

std::string dblquotewrap(std::string val){
    std::string quote("\"");
    return quote + val + quote;
}
std::string tab(int i = 1){
    return std::string(i, '\t');
}

std::string json_pair(std::string key, std::string val){
    return dblquotewrap(key) + ": " + val;
}

int main(int argc, char** argv){
    Cpu_Worker_Impl worker;

    int mwip_count = 1000;
    if(argc == 2){
        //First Args is run_count
        mwip_count = std::atoi(argv[1]);
    }

    auto start = std::chrono::steady_clock::now();
    int result = worker.MWIP(mwip_count);
    auto end = std::chrono::steady_clock::now();
    
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    auto mwip_per_second = (double)mwip_count * ((double)1000000 / (double)ms.count());

    std::string newline("\n");

    std::string output = "{" + newline;
    output += tab() + json_pair("MWIP_PER_SECOND", std::to_string(mwip_per_second)) + ", " + newline;
    output += tab() + json_pair("MWIP_OPERATIONS", std::to_string(mwip_count)) + ", " + newline;
    output += tab() + json_pair("TIME_IN_MILLISECOND", std::to_string(ms.count())) + newline;
    output += "}";
    std::cout << output << std::endl;
}
