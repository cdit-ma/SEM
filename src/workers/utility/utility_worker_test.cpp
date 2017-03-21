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

    std::cout << ute.EvaluateComplexity("cos(13*i*(2*pi)/L)", 1.0, 64.0) << std::endl;

    return 0;
}
