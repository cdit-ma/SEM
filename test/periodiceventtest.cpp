#include <iostream>

#include "../src/core/periodiceventport.h"
#include "../src/core/component.h"



static int test_count_ = 0;
bool run_test(const int test_time_ms, const double periodic_hz, const int callback_time_ms, const int expected_ticks, const double error_confidence = 0.95){
    int test_count =  ++test_count_;
    std::cout << "Test #" << test_count << " " << std::flush;

    //Calculate the upper and lower bounds of of the expected results
    auto lower = expected_ticks * error_confidence;
    auto upper  = expected_ticks * (1 + (1 - error_confidence));

    int tick_count = 0;
    {
        //Construct a Periodic Event Port with the correct options
        PeriodicEventPort pe(0, "PeriodicEvent", [&tick_count, callback_time_ms](BaseMessage* m){
                std::this_thread::sleep_for(std::chrono::milliseconds(callback_time_ms));
                tick_count++;
            });
        
        //Set the frequency, and startup the Periodic Event
        pe.SetFrequency(periodic_hz);
        pe.Startup(std::map<std::string, ::Attribute*>());

        //Activate the Periodic Event
        pe.Activate();
        //Run for the desired test length
        std::this_thread::sleep_for(std::chrono::milliseconds(test_time_ms));
        //Destructor of the PE will passivate and teardown the process
    }

    //Compare the number of the ticks to our range
    auto result = tick_count > lower && tick_count < upper;
    
    std::cout << (result ? "[SUCCESS]" : "[FAIL]") << std::endl;
    if(!result){
        std::cout << "  *Test Duration: " << test_time_ms << "ms" << std::endl;
        std::cout << "  *PE Frequency: " << periodic_hz << "hz" << std::endl;
        std::cout << "  *PE Work Duration: " << callback_time_ms << "ms" << std::endl;
        std::cout << "  *Expected Ticks: " << lower << " < t < " << upper << std::endl;
        std::cout << "  *Actual Ticks: " << tick_count << std::endl;
    }
    return result;
}

int main(int argc, char** argv){
    auto tests_success = 0;
    auto confidence_interval = 0.95;
    std::cout << "** Confidence Interval: "  << confidence_interval << std::endl;
    
    

    std::cout << "** Running Basic Tests" << std::endl;
    tests_success += run_test(1000, 1, 0, 1, confidence_interval);
    tests_success += run_test(1000, 1, 500, 1, confidence_interval);

    //Test workload which should take long than our frequency
    tests_success += run_test(2000, 1, 1500, 1, confidence_interval);
    
    //Test some power of two hz
    for(auto hz : {1, 2, 4, 8, 16, 32, 64}){
        std::cout << "** Running " << hz << "hz Tests" << std::endl;
        for(auto time : {1, 2}){
            for(auto workload : {0.0, 0.25, .50, .75, .95}){
                auto workload_length = time * workload;
                tests_success += run_test(time * 1000, hz, workload_length, hz * time, confidence_interval);
            }
        }
    }


    auto result = tests_success == test_count_;
    std::cout << "Tests " << tests_success << "/" << test_count_ << " Result: " << (result ? "SUCCESS" : "FAIL") << std::endl;
}