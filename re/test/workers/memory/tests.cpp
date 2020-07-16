
#include "component.h"
#include "memory_worker.h"
#include "memory_worker_impl.h"

#include "gtest/gtest.h"
#include <limits>
#include <chrono>
#include <memory>

#include <iostream>

struct AllocTestParam {
    AllocTestParam(size_t total_kb_alloc_, size_t num_allocs_, size_t total_kb_dealloc_, size_t num_deallocs_) :
        total_kb_alloc(total_kb_alloc_),
        num_allocs(num_allocs_),
        total_kb_dealloc(total_kb_dealloc_),
        num_deallocs(num_deallocs_)
    { }
    AllocTestParam(size_t total_kb_alloc_, size_t total_kb_dealloc_) :
        AllocTestParam(total_kb_alloc_, 1, total_kb_dealloc_, 1)
    { }

    size_t total_kb_alloc;
    size_t num_allocs;
    size_t total_kb_dealloc;
    size_t num_deallocs;
};

class MemoryTestFixture: public ::testing::TestWithParam<AllocTestParam> {
    public:
        MemoryTestFixture():
        component_("Component"),
        worker_(component_, "Component")
        {
        }

    protected:
        Component component_;
        Memory_Worker worker_;
};

std::vector<AllocTestParam> getMatchingAllocParams() {
    std::vector<AllocTestParam> params;

    params.emplace_back(1,1);
    params.emplace_back(1,1);
    params.emplace_back(32,32);
    params.emplace_back(1024,1024); // 1 MB
    params.emplace_back(1048576,1048576); // 1 GB

    return params;
}

TEST_P(MemoryTestFixture, MatchingAllocTest) {
    auto& param = GetParam();

    size_t avg_alloc_size = param.total_kb_alloc / param.num_allocs;
    size_t avg_dealloc_size = param.total_kb_dealloc / param.num_deallocs;

    size_t alloc_size_remaining = param.total_kb_alloc;
    size_t dealloc_size_remaining = param.total_kb_dealloc;

    while (alloc_size_remaining >= avg_alloc_size) {
        worker_.Allocate(avg_alloc_size);
        alloc_size_remaining -= avg_alloc_size;
    }
    if (alloc_size_remaining > 0) {
        worker_.Allocate(alloc_size_remaining);
    }

    // Should have allocated all the intended memory by now
    auto amt_allocated = worker_.GetAllocatedCount();
    EXPECT_TRUE(amt_allocated == param.total_kb_alloc);

    while (dealloc_size_remaining >= avg_dealloc_size) {
        worker_.Deallocate(avg_dealloc_size);
        dealloc_size_remaining -= avg_dealloc_size;
    }
    if (dealloc_size_remaining > 0) {
        worker_.Deallocate(dealloc_size_remaining);
    }

    // Should have deallocated all the intended memory by now
    amt_allocated = worker_.GetAllocatedCount();
    EXPECT_EQ(amt_allocated, param.total_kb_alloc - param.total_kb_dealloc);
}

TEST(Re_Workers_MemoryWorker, NegativeAllocationSmall) {
    Component comp("Component");
    Memory_Worker worker(comp, "Worker");

    worker.Allocate(-1);
    // Allocation should not occur, therefore no memory should have been allocated
    EXPECT_EQ(worker.GetAllocatedCount(), 0);
}

TEST(Re_Workers_MemoryWorker, ZeroAllocation) {
    Component comp("Component");
    Memory_Worker worker(comp, "Worker");

    worker.Allocate(0);
    EXPECT_EQ(worker.GetAllocatedCount(), 0);
}

TEST(Re_Workers_MemoryWorker, ZeroDeallocation) {
    Component comp("Component");
    Memory_Worker worker(comp, "Worker");
    worker.Deallocate(0);
    EXPECT_EQ(worker.GetAllocatedCount(), 0);
}

TEST(Re_Workers_MemoryWorker, ZeroDeallocationAfterAllocation) {
    Component comp("Component");
    Memory_Worker worker(comp, "Worker");

    worker.Allocate(10);
    EXPECT_EQ(worker.GetAllocatedCount(), 10);

    worker.Deallocate(0);
    EXPECT_EQ(worker.GetAllocatedCount(), 10);

}

TEST(Re_Workers_MemoryWorker, NegativeAllocationLarge) {
    Component comp("Component");
    Memory_Worker worker(comp, "Worker");

    worker.Allocate(-100000);
    EXPECT_EQ(worker.GetAllocatedCount(), 0);
}

TEST(Re_Workers_MemoryWorker, NegativeDeallocationSmall) {
    Component comp("Component");
    Memory_Worker worker(comp, "Worker");

    worker.Deallocate(-1);

    // Deallocation should not occur, therefore memory allocation should remain at 0
    EXPECT_EQ(worker.GetAllocatedCount(), 0);

    size_t kb_allocated = 10;

    worker.Allocate(kb_allocated);

    worker.Deallocate(-1);

    // Deallocation should not occur, therefore memory should equal amount allocated
    EXPECT_EQ(worker.GetAllocatedCount(), kb_allocated);
}

TEST(Re_Workers_MemoryWorker, NegativeDeallocationLarge) {
    Component comp("Component");
    Memory_Worker worker(comp, "Worker");

    // Deallocation should not occur, therefore memory allocation should remain at 0
    EXPECT_EQ(worker.GetAllocatedCount(), 0);

    size_t kb_allocated = 10;

    worker.Allocate(kb_allocated);

    worker.Deallocate(-100000);

    // Allocation should not occur, thereforew no memory should equal amount allocated
    EXPECT_EQ(worker.GetAllocatedCount(), kb_allocated);
}

INSTANTIATE_TEST_CASE_P(Re_Workers_MemoryWorker, MemoryTestFixture, ::testing::ValuesIn(getMatchingAllocParams()));