
#include <core/component.h>
#include <workers/utility/utility_worker.h>

#include "gtest/gtest.h"
#include <limits>
#include <chrono>
#include <memory>

TEST(UtilityWorkerEvaluateComplexity, LongFunction) {
    auto c = std::make_shared<Component>("Test");
    Utility_Worker worker(*c, "worker");

    auto result = worker.EvaluateComplexity("a + a + a + a + a + a + a + a + a + a + a + a + a + a + a + a", (double)(10));
    EXPECT_DOUBLE_EQ(result, 160);
}

TEST(UtilityWorkerEvaluateComplexity, SimpleAddition) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto result = worker.EvaluateComplexity("a + a", (double)(10));
    EXPECT_DOUBLE_EQ(result, 20);
}

TEST(UtilityWorkerEvaluateComplexity, MultiVariableFunction) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto result = worker.EvaluateComplexity("a + b", (double)(10), (double)(5));
    EXPECT_DOUBLE_EQ(result, 15);
}

TEST(UtilityWorkerEvaluateComplexity, MultiRepeatedVariableFunction) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto result = worker.EvaluateComplexity("a + b + a", (double)(10), (double)(5));
    EXPECT_DOUBLE_EQ(result, 25);
}

TEST(UtilityWorkerEvaluateComplexity, LongTokenFunction) {
    auto c = std::make_shared<Component>("Test");
    Utility_Worker worker(*c, "worker");

    auto result = worker.EvaluateComplexity("sqrt(x) + floor(x)", (double)(16));
    EXPECT_DOUBLE_EQ(result, 20);
}

TEST(UtilityWorkerEvaluateComplexity, FunctionWithAllReservedSymbols) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto result = worker.EvaluateComplexity("(b * a^a)/a + a - b", (double)(10), (double)(5));
    EXPECT_DOUBLE_EQ(result, 6245);
}

TEST(UtilityWorkerEvaluateComplexity, FunctionsWithReservedConstants) {
    auto component = std::make_shared<Component>("TestComponent");
    Utility_Worker worker(*component, "worker");

    auto pi_result = worker.EvaluateComplexity("a * pi", (double)(10));
    EXPECT_DOUBLE_EQ(pi_result, 31.415926535897931);

    //This value for epsilon is pulled from the exprtk docs
    auto eps_result = worker.EvaluateComplexity("a * epsilon", (double)(1));
    EXPECT_DOUBLE_EQ(eps_result, 0.0000000001);
}