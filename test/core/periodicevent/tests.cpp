#include "gtest/gtest.h"

#include <core/ports/periodicport.h>

//Include the FSM Tester
#include "../activatablefsmtester.h"


void empty_callback(){};

class Re_PeriodicPort_FSM_0hz : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            c = std::make_shared<Component>();
            c->RegisterPeriodicCallback(port_name, empty_callback);
            const auto& callback = c->GetCallback<void, BaseMessage>(port_name);
            auto port = new PeriodicPort(c, port_name, callback);
            port->SetFrequency(0);
            a = port;
            ASSERT_TRUE(a);
        };
 
        std::shared_ptr<Component> c;
};

class Re_PeriodicPort_FSM_1hz : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            c = std::make_shared<Component>();
            c->RegisterPeriodicCallback(port_name, empty_callback);
            const auto& callback = c->GetCallback<void, BaseMessage>(port_name);
            auto port = new PeriodicPort(c, port_name, callback);
            a = port;
            ASSERT_TRUE(a);
        };
         std::shared_ptr<Component> c;
};



//Define our Test Case Struct
struct PeriodTestCase{
    PeriodTestCase(const int test_time_ms, const double periodic_hz, const double workload_perc, const int expected_ticks, const double error_confidence = 0.95){
        this->test_time_ms = test_time_ms;
        
        this->periodic_hz = periodic_hz;
        this->expected_ticks = expected_ticks;
        this->error_confidence = error_confidence;
        this->workload_perc = workload_perc;

        //Calculate 
        lower = floor(expected_ticks * error_confidence);
        upper  = ceil(expected_ticks * (1 + (1 - error_confidence)));

        if(periodic_hz > 0){
            callback_time_ms = static_cast<size_t>((1000.0 / (double) periodic_hz) * workload_perc);
        }else{
            callback_time_ms = 0;
        }
    }
    size_t test_time_ms;
    double periodic_hz;
    size_t callback_time_ms;
    size_t expected_ticks;
    double workload_perc;
    double error_confidence;
    int lower;
    int upper;

};
std::ostream& operator<<(std::ostream& os, const PeriodTestCase& p) {
    return os << "Duration: " << p.test_time_ms << "ms, Hz=" << p.periodic_hz << "hz, Callback: " << p.workload_perc << " = " << p.callback_time_ms << "ms, Lower Bound: " << p.lower << ", Upper Bound: " << p.upper;
};

//Define a frame
class PeriodicEventTest : public ::testing::TestWithParam<PeriodTestCase>{};

//This is our test case for the frame
TEST_P(PeriodicEventTest, Re_PeriodicPort)
{
    auto test_name = get_long_test_name();
    //Get our test structure
    auto p = GetParam();

    RecordProperty("test_time_ms", p.test_time_ms);
    RecordProperty("periodic_hz", p.periodic_hz);
    RecordProperty("callback_time_ms", p.callback_time_ms);
    RecordProperty("expected_ticks", p.expected_ticks);

   int callback_tick_count = 0;
   {
        auto c = std::make_shared<Component>(test_name);
        c->RegisterPeriodicCallback(test_name, [&callback_tick_count, &p](void){
            std::this_thread::sleep_for(std::chrono::milliseconds(p.callback_time_ms));
            callback_tick_count ++;
        });

        const auto& callback = c->GetCallback<void, BaseMessage>(test_name);
        PeriodicPort port(c, test_name, callback);

        //Set the frequency, and startup the Periodic Event
        port.SetFrequency(p.periodic_hz);
        EXPECT_TRUE(port.Configure());
        EXPECT_TRUE(port.Activate());

        //Run for the desired test length
        std::this_thread::sleep_for(std::chrono::milliseconds(p.test_time_ms));
        EXPECT_TRUE(port.Passivate());
        EXPECT_TRUE(port.Terminate());

        auto total_rxd = port.GetEventsReceived();
        auto proc_rxd = port.GetEventsProcessed();

        //Check that we got the same number of proccessed rx messages as we did in our callback
        EXPECT_EQ(proc_rxd, callback_tick_count);
   }

   //The number of callbacks we got should fall within the range of satisfactory
   EXPECT_GE(callback_tick_count, p.lower);
   EXPECT_LE(callback_tick_count, p.upper);
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


#define TEST_FSM_CLASS Re_PeriodicPort_FSM_0hz
#include "../activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS Re_PeriodicPort_FSM_1hz
#include "../activatablefsmtestcases.h"
#undef TEST_FSM_CLASS



INSTANTIATE_TEST_CASE_P(Re_PeriodicPort_0hz_5s_LONG, PeriodicEventTest, ::testing::ValuesIn(getTestCases(0, 5, 1)));
INSTANTIATE_TEST_CASE_P(Re_PeriodicPort_1Hz_5s_LONG, PeriodicEventTest, ::testing::ValuesIn(getTestCases(1, 5)));
INSTANTIATE_TEST_CASE_P(Re_PeriodicPort_2Hz_5s_LONG, PeriodicEventTest, ::testing::ValuesIn(getTestCases(2, 5)));
INSTANTIATE_TEST_CASE_P(Re_PeriodicPort_4Hz_5s_LONG, PeriodicEventTest, ::testing::ValuesIn(getTestCases(4, 5)));
INSTANTIATE_TEST_CASE_P(Re_PeriodicPort_8Hz_5s_LONG, PeriodicEventTest, ::testing::ValuesIn(getTestCases(8, 5)));
INSTANTIATE_TEST_CASE_P(Re_PeriodicPort_16Hz_5s_LONG, PeriodicEventTest, ::testing::ValuesIn(getTestCases(16, 5)));
INSTANTIATE_TEST_CASE_P(Re_PeriodicPort_32Hz_5s_LONG, PeriodicEventTest, ::testing::ValuesIn(getTestCases(32, 5, 0.70)));