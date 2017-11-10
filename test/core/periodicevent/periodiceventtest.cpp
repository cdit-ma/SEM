#include <iostream>
#include <ostream>
#include <core/eventports/periodiceventport.h>
#include <core/component.h>
#include "gtest/gtest.h"




class StateMachineTest : public ::testing::Test {
protected:
    void SetUp(){
        auto p = new PeriodicEventPort(nullptr, "PeriodicEvent", [](BaseMessage*){});
        p->SetFrequency(1);
        a = p;
        ASSERT_TRUE(a);
    }
    void TearDown(){
        if(a){
            delete a;
        }
    }
    void sleep_ms(int ms){
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    };
    
    int random_sleep_ms(int lower_bound, int upper_bound){
       // std::uniform_int_distribution<int> distribution(lower_bound, upper_bound);
        //auto val = distribution(random_generator);
        sleep_ms(1);
        return 1;
    }

    Activatable* a = 0;
    //std::default_random_engine random_generator(Testing::Test::random_seed());
};

#define MACRO_CLASS_NAME StateMachineTest
#include "../../core/state_machine_tests.h"



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

        if(periodic_hz > 0){
            callback_time_ms = (1000.0 / (double) periodic_hz) * workload_perc;
        }else{
            callback_time_ms = 0;
        }
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

   int callback_tick_count = 0;
   {
       auto pe = std::make_shared<PeriodicEventPort>(nullptr, "PeriodicEvent", [&callback_tick_count, p](BaseMessage* m){
               std::this_thread::sleep_for(std::chrono::milliseconds(p.callback_time_ms));
               callback_tick_count ++;
           });

       //Set the frequency, and startup the Periodic Event
       pe->SetFrequency(p.periodic_hz);
       pe->Configure();
       pe->Activate();

       //Run for the desired test length
       std::this_thread::sleep_for(std::chrono::milliseconds(p.test_time_ms));
       pe->Passivate();
       pe->Terminate();

       auto total_rxd = pe->GetEventsReceieved();
       auto proc_rxd = pe->GetEventsProcessed();

       //Check that we got the same number of proccessed rx messages as we did in our callback
       EXPECT_EQ(proc_rxd, callback_tick_count);
   }

   //The number of callbacks we got should fall within the range of satisfactory
   ASSERT_GE(callback_tick_count, p.lower);
   ASSERT_LE(callback_tick_count, p.upper);
}

//Define a helper to generate a range of test cases for a particular hz/time/confidence interval
std::vector<PeriodTestCase> getTestCases(int hz, double time, double confidence_interval = 0.95) 
{
    std::vector<PeriodTestCase> test_cases;
    for(auto workload : {0.0, 0.50, .75}){
        auto time_ms = time * 1000;
        auto expected_ticks = (hz * time);
        //We have a sleep at the start
        if(expected_ticks > 0){
            expected_ticks --;
        }
        auto test_case = PeriodTestCase(time_ms, hz, workload, expected_ticks, confidence_interval);
        test_cases.push_back(test_case);
    }
    return test_cases;
};

//Define a helper to generate a range of test cases for a particular hz/time/confidence interval
std::vector<Activatable*> getTypes() 
{
    std::vector<Activatable*> test_cases;

    auto a = new PeriodicEventPort(nullptr, "PeriodicEvent", nullptr);
    test_cases.push_back(a);
    return test_cases;
};

//INSTANTIATE_TEST_CASE_P(PEP, StateMachineTest, ::testing::ValuesIn(getTypes()));
//INSTANTIATE_TEST_CASE_P(1Hz_5s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(1, 5)));
/*
INSTANTIATE_TEST_CASE_P(2Hz_5s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(2, 5)));
INSTANTIATE_TEST_CASE_P(4Hz_5s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(4, 5)));
INSTANTIATE_TEST_CASE_P(8Hz_5s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(8, 5)));
INSTANTIATE_TEST_CASE_P(16Hz_5s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(16, 5)));
INSTANTIATE_TEST_CASE_P(32Hz_5s, PeriodicEventTest, ::testing::ValuesIn(getTestCases(32, 5, 0.70)));
*/


int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}