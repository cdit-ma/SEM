#include <iostream>

#include "../src/core/periodiceventport.h"
#include "../src/core/component.h"



static int test_count_ = 0;
bool run_test(const int test_time_ms, const int period_time_ms, const int callback_time_ms, const int expected_ticks){
    int test_count =  ++test_count_;
    std::cout << "Test #" << test_count << ":" << std::endl;
    std::cout << "  *Test Duration: " << test_time_ms << "ms" << std::endl;
    std::cout << "  *Periodic Event Duration: " << period_time_ms << "ms" << std::endl;
    std::cout << "  *Periodic Event Callback Duration: " << callback_time_ms << "ms" << std::endl;
    std::cout << "  *Expecting: " << expected_ticks << " Ticks" << std::endl;
    std::cout << "Result: " << std::flush;
    int tick_count = 0;

    auto component = new Component();
    bool should_sleep = callback_time_ms > 0;
    auto sleep_time = std::chrono::milliseconds(callback_time_ms);
    auto periodic = new PeriodicEventPort(component, "Periodic", [&tick_count, sleep_time, should_sleep](BaseMessage* m){
        if(should_sleep)
            std::this_thread::sleep_for(sleep_time);
        tick_count++;});

    periodic->SetDuration(period_time_ms);
    periodic->Startup(std::map<std::string, ::Attribute*>());

    component->Activate();
    std::this_thread::sleep_for(std::chrono::milliseconds(test_time_ms));
    component->Passivate();
    component->Teardown();


    auto result = tick_count == expected_ticks;
    std::cout << (result ? "SUCCESS" : "FAIL") << " Tick: " << tick_count << std::endl << std::endl;
    delete component;
    return result;
}

int main(int argc, char** argv){
    auto tests_success = 0;
    tests_success += run_test(2000, 1000, 0, 2);
    tests_success += run_test(2000, 1000, 1001, 1);
    tests_success += run_test(2000, 100, 0, 20);
    tests_success += run_test(2000, 10, 0, 100);

    auto result = tests_success == test_count_;
    std::cout << "Tests " << tests_success << "/" << test_count_ << " Result: " << (result ? "SUCCESS" : "FAIL") << std::endl;
}