//
// Created by Jackson Michael on 7/5/20.
//

#include <gtest/gtest.h>

#include <cmath>
#include "component.h"
#include "loggers/print_logger.h"
#include <cudaworker.h>
#include <devicelist.cuh>

namespace cditma::workers::cuda::test {

TEST(Re_Workers_CUDAWorker, Constructor_Destructor)
{
    // Create a worker, then let it fall out of scope immediately
    Component test_component("test_component");
    ASSERT_NO_THROW(CUDAWorker worker(test_component, "constructor_test_worker"));
}

TEST(Re_Workers_CUDAWorker, Basic_FFT)
{
    // Instantiate a worker and hook it up to a printer that will let us see errors
    Component test_component("test_component");
    CUDAWorker worker(test_component, "fft_test_worker");
    worker.logger().AddLogger(Print::Logger::get_logger());

    // Initialise the worker
    bool did_configure = false;
    ASSERT_NO_THROW(did_configure = worker.Configure());
    ASSERT_TRUE(did_configure);

    // 64 interleaved complex samples of a 16Hz frequency at one sample per second
    // clang-format off
    std::vector<float> in_data{1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0,
                               1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0,
                               1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0,
                               1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0,
                               1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0,
                               1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0,
                               1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0,
                               1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0};
    // clang-format on

    // Perform the FFT, check that it succeeded and that we actually got any data
    bool did_succeed = false;
    std::vector<float> out_data;
    ASSERT_NO_THROW(out_data = worker.FFT(in_data, did_succeed););
    ASSERT_TRUE(did_succeed);
    ASSERT_EQ(in_data.size(), out_data.size());

    // Expect to see a spike at 16 Hz, and another mirrored around the middle of the output
    EXPECT_EQ(out_data[16*2], 32);
    EXPECT_EQ(out_data[(64-16)*2], 32);

    // Make sure that we are able to shut down properly
    bool did_terminate;
    ASSERT_NO_THROW(did_terminate = worker.Terminate());
    ASSERT_TRUE(did_terminate);
}

} // namespace cditma::workers::cuda::test
