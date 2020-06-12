#ifndef ACTIVATABLE_FSM_TESTER_H
#define ACTIVATABLE_FSM_TESTER_H

#include <random>
#include <core/component.h>

std::string get_long_test_name(){
    auto test_info = testing::UnitTest::GetInstance()->current_test_info();
    return test_info->test_case_name() + std::string("_") + test_info->name();
}

class ActivatableFSMTester : public ::testing::Test {
    protected:
        std::shared_ptr<Component> component;
        
        Port* a = 0;
        std::default_random_engine random_generator;

        virtual void SetUp(){
            random_generator.seed(testing::UnitTest::GetInstance()->random_seed());
            component = std::make_shared<Component>(get_long_test_name());
        }

        void TearDown(){
            if(a){
                delete a;
            }
        }
};

#endif //ACTIVATABLE_FSM_TESTER_H