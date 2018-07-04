
#include <core/component.h>
#include <workers/memory/memory_worker.h>

#include "gtest/gtest.h"
#include <limits>
#include <chrono>
#include <memory>


struct AllocTestParam {
    AllocTestParam(size_t total_kb_alloc_, size_t num_allocs_, size_t total_kb_dealloc_, size_t num_deallocs_) :
        total_kb_alloc(total_kb_alloc_),
        num_allocs(num_allocs_),
        total_kb_dealloc(total_kb_dealloc_),
        num_deallocs(num_deallocs_) {};
    AllocTestParam(size_t total_kb_alloc_, size_t total_kb_dealloc_) :
        total_kb_alloc(total_kb_alloc_),
        num_allocs(1),
        total_kb_dealloc(total_kb_dealloc_),
        num_deallocs(1) {};

    size_t total_kb_alloc;
    size_t num_allocs;
    size_t total_kb_dealloc;
    size_t num_deallocs;
};

class MemoryTestFixture: public ::testing::TestWithParam<AllocTestParam> {
    public:
        MemoryTestFixture() /*: OpenCLWorkerConstructor(GetParam().device)*/{
            if(!worker_.Configure()){
                throw std::runtime_error("Failed to configure worker in MatrixMultFixture constructor");
            }
        }

    private:
        Component component_ = Component("component");
        Memory_Worker worker_ = Memory_Worker(component_, "MemWorker");
};

std::vector<AllocTestParam> getMatchingAllocParams() {
    std::vector<AllocTestParam> params;

    params.emplace_back(1,1);
    params.emplace_back(1,1);
}

TEST_P(MemoryTestFixture, MatchingAllocTest) {
    auto c = std::make_shared<Component>("Test");
    Memory_Worker worker(*c, "worker");
    auto& param = GetParam();

    size_t avg_alloc_size = param.total_kb_alloc / param.num_allocs;
    size_t avg_dealloc_size = param.total_kb_dealloc / param.num_deallocs;

    size_t alloc_size_remaining = param.total_kb_alloc;
    size_t dealloc_size_remaining = param.total_kb_dealloc;

    //auto start = std::chrono::steady_clock::now();
    //auto result = worker.FloatOp(run_count);

    while (alloc_size_remaining > avg_alloc_size) {
        worker.Allocate(avg_alloc_size);
    }
    if (alloc_size_remaining > 0) {
        worker.Allocate(alloc_size_remaining);
    }

    auto amt_allocated = worker.GetAllocatedCount();
    EXPECT_TRUE(amt_allocated == param.total_kb_alloc);

    while (dealloc_size_remaining > avg_dealloc_size) {
        worker.Deallocate(avg_dealloc_size);
    }
    if (dealloc_size_remaining > 0) {
        worker.Deallocate(dealloc_size_remaining);
    }

    amt_allocated = worker.GetAllocatedCount();
    EXPECT_TRUE(amt_allocated, 0);

    //auto end = std::chrono::steady_clock::now();
    //auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    //EXPECT_TRUE(result == 0);
    //EXPECT_GT(ms.count(), 100)
}

INSTANTIATE_TEST_CASE_P(AllocDeallocPair, MemoryTestFixture, ::testing::ValuesIn(getMatchingAllocParams()))