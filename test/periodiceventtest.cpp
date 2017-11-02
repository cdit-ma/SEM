#include <iostream>
#include <ostream>
#include "../src/core/periodiceventport.h"
#include "../src/core/component.h"
#include "gtest/gtest.h"

//Define our Test Case Struct
struct PeriodTestCase{
    PeriodTestCase(const int test_time_ms, const double periodic_hz, const double workload_perc, const int expected_ticks, const double error_confidence = 0.95){
        this->test_time_ms = test_time_ms;
        this->periodic_hz = periodic_hz;
        this->expected_ticks = expected_ticks;
        this->error_confidence = error_confidence;
        this->workload_perc = workload_perc;

        //Calculate 
        lower = expected_ticks * error_confidence;
        upper  = expected_ticks * (1 + (1 - error_confidence));
        callback_time_ms = (1000.0 / (double) periodic_hz) * workload_perc;
    }
    int test_time_ms;
    int periodic_hz;
    int callback_time_ms;
    int expected_ticks;
    double workload_perc;
    double error_confidence;
    double lower;
    double upper;

};
std::ostream& operator<<(std::ostream& os, const PeriodTestCase& p) {
    return os << "Duration: " << p.test_time_ms << "ms, Hz=" << p.periodic_hz << "hz, Callback: " << p.workload_perc << " = " << p.callback_time_ms << "ms, Lower Bound: " << p.lower << ", Upper Bound: " << p.upper;
};

//Define a frame
class PeriodicEventTest : public ::testing::TestWithParam<PeriodTestCase>{};

//This is our test case for the frame
TEST_P(PeriodicEventTest, TickCount)
{
    //Get our test structure
    auto p = GetParam();

    RecordProperty("test_time_ms", p.test_time_ms);
    RecordProperty("periodic_hz", p.periodic_hz);
    RecordProperty("callback_time_ms", p.callback_time_ms);
    RecordProperty("expected_ticks", p.expected_ticks);
    
   int tick_count = 0;
   {
       //Construct a Periodic Event Port with the correct options
       PeriodicEventPort pe(0, "PeriodicEvent", [&tick_count, p](BaseMessage* m){
               std::this_thread::sleep_for(std::chrono::milliseconds(p.callback_time_ms));
               tick_count++;
           });
       
       //Set the frequency, and startup the Periodic Event
       pe.SetFrequency(p.periodic_hz);
       pe.Startup(std::map<std::string, ::Attribute*>());
       //Activate the Periodic Event
       pe.Activate();
       //Run for the desired test length
       std::this_thread::sleep_for(std::chrono::milliseconds(p.test_time_ms));
       //Destructor of the PE will passivate and teardown the process
   }

   ASSERT_GT(tick_count, p.lower);
   ASSERT_LT(tick_count, p.upper);
}

//Define a helper to generate a range of test cases for a particular hz/time/confidence interval
std::vector<PeriodTestCase> getTestCases(int hz, double time, double confidence_interval = 0.95) 
{
    std::vector<PeriodTestCase> test_cases;
    for(auto workload : {0.0, 0.25, .50, .75, .95}){
        auto time_ms = time * 1000;
        auto expected_ticks = hz * time;
        auto test_case = PeriodTestCase(time_ms, hz, workload, expected_ticks, confidence_interval);
        test_cases.push_back(test_case);
    }
    return test_cases;
};



INSTANTIATE_TEST_CASE_P(1Hz_1s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(1, 1)));
INSTANTIATE_TEST_CASE_P(1Hz_2s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(1, 2)));
INSTANTIATE_TEST_CASE_P(1Hz_4s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(1, 4)));
INSTANTIATE_TEST_CASE_P(1Hz_8s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(1, 8)));

INSTANTIATE_TEST_CASE_P(2Hz_1s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(2, 1)));
INSTANTIATE_TEST_CASE_P(2Hz_2s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(2, 2)));
INSTANTIATE_TEST_CASE_P(2Hz_4s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(2, 4)));
INSTANTIATE_TEST_CASE_P(2Hz_8s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(2, 8)));

INSTANTIATE_TEST_CASE_P(4Hz_1s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(4, 1)));
INSTANTIATE_TEST_CASE_P(4Hz_2s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(4, 2)));
INSTANTIATE_TEST_CASE_P(4Hz_4s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(4, 4)));
INSTANTIATE_TEST_CASE_P(4Hz_8s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(4, 8)));

INSTANTIATE_TEST_CASE_P(8Hz_1s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(8, 1)));
INSTANTIATE_TEST_CASE_P(8Hz_2s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(8, 2)));
INSTANTIATE_TEST_CASE_P(8Hz_4s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(8, 4)));
INSTANTIATE_TEST_CASE_P(8Hz_8s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(8, 8)));

INSTANTIATE_TEST_CASE_P(16Hz_1s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(16, 1)));
INSTANTIATE_TEST_CASE_P(16Hz_2s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(16, 2)));
INSTANTIATE_TEST_CASE_P(16Hz_4s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(16, 4)));
INSTANTIATE_TEST_CASE_P(16Hz_8s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(16, 8)));


INSTANTIATE_TEST_CASE_P(32Hz_8s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(64, 8, 0.90)));
INSTANTIATE_TEST_CASE_P(64Hz_8s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(64, 8, 0.80)));

int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}