#include <core/activatable.h>

//Requires a subclass of ActivatableFSMTester to be defined, and then this header file to be included with TEST_FSM_CLASS defined

/*
#define TEST_FSM_CLASS ZeroMQ_InEventPort
#include "activatablefsmtestcases.h"
#undef TEST_FSM_CLASS
*/

/*
    State: NOT_CONFIGURED

    Valid Transitions:
        Configure() - C
    Invalid Transitions:
        Activate()  - A
        Passivate() - P
        Terminate() - T
*/

TEST_F(TEST_FSM_CLASS, V_C)
{
    ASSERT_TRUE(a->Configure());
};


TEST_F(TEST_FSM_CLASS, I_A)
{
    ASSERT_FALSE(a->Activate());
};

TEST_F(TEST_FSM_CLASS, I_P)
{
    ASSERT_FALSE(a->Passivate());
};

TEST_F(TEST_FSM_CLASS, I_T)
{
    ASSERT_FALSE(a->Terminate());
};

/*
    State: CONFIGURED

    Valid Transitions:
        Activate()  - CA
        Terminate() - CT
    Invalid Transitions:
        Configure() - CC
        Passivate() - CP
*/

TEST_F(TEST_FSM_CLASS, V_CA)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
};

TEST_F(TEST_FSM_CLASS, V_CT)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Terminate());
};

TEST_F(TEST_FSM_CLASS, I_CC)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_FALSE(a->Configure());
};

TEST_F(TEST_FSM_CLASS, V_CP)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
};

/*
    State: RUNNING

    Valid Transitions:
        Passivate() - CAP
        Terminate() - CAT
    Invalid Transitions:
        Activate()  - CAA
        Configure() - CAC
*/

TEST_F(TEST_FSM_CLASS, V_CAP)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
};

TEST_F(TEST_FSM_CLASS, V_CAT)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Terminate());
};

TEST_F(TEST_FSM_CLASS, I_CAA)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_FALSE(a->Activate());
};

TEST_F(TEST_FSM_CLASS, I_CAC)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_FALSE(a->Configure());
};

/*
    State: NOT_RUNNING

    Valid Transitions:
        Terminate() - CAPT
    Invalid Transitions:
        Activate()  - CAPA
        Configure() - CAPC
        Passivate() - CAPP
*/

TEST_F(TEST_FSM_CLASS, V_CAPT)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_TRUE(a->Terminate());
};

TEST_F(TEST_FSM_CLASS, I_CAPA)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_FALSE(a->Activate());
};

TEST_F(TEST_FSM_CLASS, I_CAPC)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_FALSE(a->Configure());
};

TEST_F(TEST_FSM_CLASS, I_CAPP)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_FALSE(a->Passivate());
};

/*
    Extra Tests
*/

TEST_F(TEST_FSM_CLASS, V_CAPTCAPT)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_TRUE(a->Terminate());

    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_TRUE(a->Terminate());
};

TEST_F(TEST_FSM_CLASS, LONG_V_CAPTx10)
{
    int count = 10;
    while(count-- > 0){
        ASSERT_TRUE(a->Configure());
        ASSERT_TRUE(a->Activate());
        ASSERT_TRUE(a->Passivate());
        ASSERT_TRUE(a->Terminate());
    }
};

TEST_F(TEST_FSM_CLASS, LONG_V_CAPTx10_FIX)
{
    int sleep_time = 5;
    int count = 10;
    while(count-- > 0){
        sleep_ms(sleep_time);
        ASSERT_TRUE(a->Configure());
        sleep_ms(sleep_time);
        ASSERT_TRUE(a->Activate());
        sleep_ms(sleep_time);
        ASSERT_TRUE(a->Passivate());
        sleep_ms(sleep_time);
        ASSERT_TRUE(a->Terminate());
    }
};

TEST_F(TEST_FSM_CLASS, LONG_V_CAPTx10_RAN)
{
    int min = 1;
    int max = 50;
    int count = 10;
    while(count-- > 0){
        random_sleep_ms(min, max);
        ASSERT_TRUE(a->Configure());
        random_sleep_ms(min, max);
        ASSERT_TRUE(a->Activate());
        random_sleep_ms(min, max);
        ASSERT_TRUE(a->Passivate());
        random_sleep_ms(min, max);
        ASSERT_TRUE(a->Terminate());
    }
};