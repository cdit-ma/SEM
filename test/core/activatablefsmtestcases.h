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

TEST_F(TEST_FSM_CLASS, Valid_C)
{
    ASSERT_TRUE(a->Configure());
};


TEST_F(TEST_FSM_CLASS, Invalid_A)
{
    ASSERT_FALSE(a->Activate());
};

TEST_F(TEST_FSM_CLASS, Invalid_P)
{
    ASSERT_FALSE(a->Passivate());
};

TEST_F(TEST_FSM_CLASS, Invalid_T)
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

TEST_F(TEST_FSM_CLASS, Valid_CA)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
};

TEST_F(TEST_FSM_CLASS, Valid_CT)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Terminate());
};

TEST_F(TEST_FSM_CLASS, Invalid_CC)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_FALSE(a->Configure());
};

TEST_F(TEST_FSM_CLASS, Valid_CP)
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

TEST_F(TEST_FSM_CLASS, Valid_CAP)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
};

TEST_F(TEST_FSM_CLASS, Valid_CAT)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Terminate());
};

TEST_F(TEST_FSM_CLASS, Invalid_CAA)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_FALSE(a->Activate());
};

TEST_F(TEST_FSM_CLASS, Invalid_CAC)
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

TEST_F(TEST_FSM_CLASS, Valid_CAPT)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_TRUE(a->Terminate());
};

TEST_F(TEST_FSM_CLASS, Invalid_CAPA)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_FALSE(a->Activate());
};

TEST_F(TEST_FSM_CLASS, Invalid_CAPC)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_FALSE(a->Configure());
};

TEST_F(TEST_FSM_CLASS, Invalid_CAPP)
{
    ASSERT_TRUE(a->Configure());
    ASSERT_TRUE(a->Activate());
    ASSERT_TRUE(a->Passivate());
    ASSERT_FALSE(a->Passivate());
};

/*
    Extra Tests
*/

TEST_F(TEST_FSM_CLASS, Valid_CAPTCAPT)
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

TEST_F(TEST_FSM_CLASS, Valid_CAPTx10)
{
    int count = 100;
    while(count-- > 0){
        ASSERT_TRUE(a->Configure());
        ASSERT_TRUE(a->Activate());
        ASSERT_TRUE(a->Passivate());
        ASSERT_TRUE(a->Terminate());
    }
};

TEST_F(TEST_FSM_CLASS, Valid_CAPTx10_DELAY)
{
    int count = 10;
    while(count-- > 0){
        sleep_ms(10);
        ASSERT_TRUE(a->Configure());
        sleep_ms(10);
        ASSERT_TRUE(a->Activate());
        sleep_ms(10);
        ASSERT_TRUE(a->Passivate());
        sleep_ms(10);
        ASSERT_TRUE(a->Terminate());
    }
};

TEST_F(TEST_FSM_CLASS, Valid_CAPTx10_RANDOMDELAY)
{
    int min = 10;
    int max = 100;
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