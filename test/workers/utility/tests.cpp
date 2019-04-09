
#include <core/component.h>
#include <workers/utility/utility_worker.h>

#include "gtest/gtest.h"
#include <limits>
#include <chrono>
#include <memory>

TEST(Re_Workers_UtilityWorker, EvaluateComplexity_LongFunction) {
    auto c = std::make_shared<Component>("Test");
    Utility_Worker worker(*c, "worker");

    auto result = worker.EvaluateComplexity("a + a + a + a + a + a + a + a + a + a + a + a + a + a + a + a", (double)(10));
    EXPECT_DOUBLE_EQ(result, 160);
}

TEST(Re_Workers_UtilityWorker, EvaluateComplexity_SimpleAddition) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto result = worker.EvaluateComplexity("a + a", (double)(10));
    EXPECT_DOUBLE_EQ(result, 20);
}

TEST(Re_Workers_UtilityWorker, EvaluateComplexity_MultiVariableFunction) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto result = worker.EvaluateComplexity("a + b", (double)(10), (double)(5));
    EXPECT_DOUBLE_EQ(result, 15);
}

TEST(Re_Workers_UtilityWorker, EvaluateComplexity_MultiRepeatedVariableFunction) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto result = worker.EvaluateComplexity("a + b + a", (double)(10), (double)(5));
    EXPECT_DOUBLE_EQ(result, 25);
}

TEST(Re_Workers_UtilityWorker, EvaluateComplexity_LongTokenFunction) {
    auto c = std::make_shared<Component>("Test");
    Utility_Worker worker(*c, "worker");

    auto result = worker.EvaluateComplexity("sqrt(x) + floor(x)", (double)(16));
    EXPECT_DOUBLE_EQ(result, 20);
}

TEST(Re_Workers_UtilityWorker, EvaluateComplexity_FunctionWithAllReservedSymbols) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto result = worker.EvaluateComplexity("(b * a^a)/a + a - b", (double)(10), (double)(5));
    EXPECT_DOUBLE_EQ(result, 6245);
}

TEST(Re_Workers_UtilityWorker, EvaluateComplexity_FunctionsWithReservedConstants) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto pi_result = worker.EvaluateComplexity("a * pi", (double)(10));
    EXPECT_DOUBLE_EQ(pi_result, 31.415926535897931);

    //This value for epsilon is pulled from the exprtk docs
    auto eps_result = worker.EvaluateComplexity("a * epsilon", (double)(1));
    EXPECT_DOUBLE_EQ(eps_result, 0.0000000001);
}

TEST(Re_Workers_UtilityWorker, GetTime) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");
    auto time = worker.GetTimeOfDay();
    auto time_str = worker.GetTimeOfDayString();
    
    EXPECT_GT(time, 0);
    EXPECT_GT(time_str.length(), 0);
}
