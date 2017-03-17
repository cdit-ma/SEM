#include "utilityworker.h"
#include <iostream>
#include <thread>
#include <chrono>
int main(){

    UtilityWorker ute;

    std::cout << ute.TimeOfDayString() << std::endl;
    std::cout << ute.TimeOfDay() << std::endl;

    double i = 1;
    double n = 10;
    double b = 10;

    std::cout << __func__ << std::endl;

    std::cout << ute.EvaluateComplexity("(n/i)*n*i*n*n", n, i) << std::endl;

    return 0;
}
