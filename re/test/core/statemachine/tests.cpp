#include <memory>
#include <random>

#include <gtest/gtest.h>
#include "statemachine.h"

class Re_Core_StateMachine_FSM: public ::testing::Test {
    protected:
        std::unique_ptr<StateMachine> a;
        std::default_random_engine random_generator;
    protected:
        void SetUp(){
            random_generator.seed(testing::UnitTest::GetInstance()->random_seed());
            a = std::unique_ptr<StateMachine>(new StateMachine());
            ASSERT_TRUE(a);
        };
        void TearDown(){
            a.reset();
        };
};

#define TEST_FSM_CLASS Re_Core_StateMachine_FSM
#include "../activatablefsmtestcases.h"
#undef TEST_FSM_CLASS