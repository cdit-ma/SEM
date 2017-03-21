#include "utility_worker.h"
#include <iostream>
#include <thread>
#include <chrono>
int main(){

    Utility_Worker ute(0, "ute");

    std::cout << ute.GetTimeOfDayString() << std::endl;
    std::cout << ute.GetTimeOfDay() << std::endl;

    double i = 1;
    double n = 10;
    double b = 10;

    std::cout << __func__ << std::endl;

    std::cout << ute.EvaluateComplexity("((n/i)*n*i*n*n)+c", n, i, 10) << std::endl;

    return 0;
}
