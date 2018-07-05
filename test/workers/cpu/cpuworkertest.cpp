
#include <core/component.h>
#include <workers/cpu/cpu_worker.h>

#include "gtest/gtest.h"
#include <limits>
#include <chrono>
#include <memory>

std::vector<float> GetIdentityMatrix(int dimension){
    auto size = dimension * dimension;
    std::vector<float> mat(size);

    for (auto i = 0; i < size; i++){
        auto r = i / dimension;
        auto c = i % dimension;
        mat[i] = r == c ? 1.0f : 0.0f;
    }
    return mat;
}

TEST(MatrixMult, Trivial) {
    auto c = std::make_shared<Component>("Test");
    Cpu_Worker worker(*c, "worker");

    std::vector<float> mat_a(2 * 2);
    std::vector<float> mat_b(2 * 2);
    std::vector<float> mat_c(2 * 2);

    std::vector<float> expected {2, 3, 6, 11};

    for (auto index=0; index<4; index++) mat_a[index] = static_cast<float>(index);
    for (auto index=0; index<4; index++) mat_b[index] = static_cast<float>(index);
    for (auto index=0; index<4; index++) mat_c[index] = static_cast<float>(index);

    auto result = worker.MatrixMult(mat_a, mat_b, mat_c);

    EXPECT_TRUE(result == 0);
    EXPECT_TRUE(mat_c == expected);
}

TEST(MatrixMult, IdentityTest) {
    auto c = std::make_shared<Component>("Test");
    Cpu_Worker worker(*c, "worker");

    std::vector<float> mat_a(2 * 2);
    std::vector<float> mat_c(2 * 2, 0);
    auto mat_i = GetIdentityMatrix(2);

    for (auto index=0; index<4; index++) mat_a[index] = static_cast<float>(index);

    auto result = worker.MatrixMult(mat_a, mat_i, mat_c);

    EXPECT_TRUE(result == 0);
    EXPECT_TRUE(mat_c == mat_a);
}


TEST(MatrixMult, LONG_Big1024) {
    auto c = std::make_shared<Component>("Test");
    Cpu_Worker worker(*c, "worker");

    std::vector<float> mat_a(1024 * 1024);
    std::vector<float> mat_b(1024 * 1024);
    std::vector<float> mat_c(1024 * 1024);

    for (unsigned int index=0; index<1024 * 1024; index++) mat_a[index] = (float)index;
    for (unsigned int index=0; index<1024 * 1024; index++) mat_b[index] = (float)index;
    for (unsigned int index=0; index<1024 * 1024; index++) mat_c[index] = std::numeric_limits<float>::signaling_NaN();

    auto start = std::chrono::steady_clock::now();
    auto result = worker.MatrixMult(mat_a, mat_b, mat_c);
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    
    //check for not-nan
    bool all_good = true;
    for(auto& ele : mat_c){
        if(ele == std::numeric_limits<float>::signaling_NaN()){
            all_good = false;
            break;
        }
    }

    EXPECT_TRUE(result == 0);
    EXPECT_TRUE(all_good);
    EXPECT_GT(ms.count(), 1000);
}

TEST(IntOp, Big) {
    auto c = std::make_shared<Component>("Test");
    Cpu_Worker worker(*c, "worker");
    double run_count = 1000000000;

    auto start = std::chrono::steady_clock::now();
    auto result = worker.IntOp(run_count);
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_TRUE(result == 0);
    EXPECT_GT(ms.count(), 100);
}

TEST(FloatOp, Big) {
    auto c = std::make_shared<Component>("Test");
    Cpu_Worker worker(*c, "worker");
    double run_count = 1000000000;

    auto start = std::chrono::steady_clock::now();
    auto result = worker.FloatOp(run_count);
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_TRUE(result == 0);
    EXPECT_GT(ms.count(), 100);
}