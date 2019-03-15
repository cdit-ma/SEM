#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

#include <random>
#include <thread>

void sleep_ms(int ms){
    if(ms > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
};

int random_sleep_ms(std::default_random_engine& random_generator, int lower_bound, int upper_bound){
    std::uniform_int_distribution<int> distribution(lower_bound, upper_bound);
    auto val = distribution(random_generator);
    sleep_ms(val);
    return val;
};

#endif //TEST_FUNCTIONS_H