#include "common.h"

namespace cdit_ma::test::opencl {

/**
 * Used to store the parameters required to test the RunParallel function:
 * - device: which device this particular test instance should target
 * - num_threads & num_ops: the input parameters for the function
 * - expect_success: whether or not the test case is expected to pass
 */
struct RunParallelParam {
    /**
     * Simple constructor that initialises struct values
     * @param device
     * @param num_threads
     * @param num_ops
     * @param expect_success
     */
    RunParallelParam(DeviceParam device,
                     size_t num_threads,
                     size_t num_ops,
                     cdit_ma::test::Result expected_result) :
        device(device),
        num_threads(num_threads),
        num_ops(num_ops),
        expected_result(expected_result)
    {
    }
    DeviceParam device;
    size_t num_threads;
    size_t num_ops;
    Result expected_result;
};

/**
 * Overloaded ostream piping operator for pretty printing RunParallelParams associated with failing
 * cases
 * @param os
 * @param rpp
 * @return
 */
std::ostream& operator<<(std::ostream& os, const RunParallelParam& rpp)
{
    return os << rpp.device << " - threads: [" << rpp.num_threads << "] - operations: ["
              << rpp.num_ops << "]";
}

/**
 * The test fixture responsible for performing required initialisation and teardown for testing the
 * RunParallel function
 */
class RunParallelFixture : public ::testing::TestWithParam<RunParallelParam>,
                           public OpenCL_WorkerConstructor {
public:
    /**
     * Constructor performs test setup by ensuring that the worker is in a configured and valid
     * state before tests run
     */
    RunParallelFixture() : OpenCL_WorkerConstructor(GetParam().device)
    {
        if(!worker_.Configure()) {
            throw std::runtime_error("Failed to configure worker in RunParallelFixture "
                                     "constructor");
        }
    }
};

TEST_P(RunParallelFixture, RunParallel)
{
    auto num_threads = GetParam().num_threads;
    auto num_ops = GetParam().num_ops;
    auto expected_result = GetParam().expected_result;

    bool did_report_success = false;
    Result actual_result = Result::failure;

    EXPECT_NO_THROW(did_report_success = worker_.RunParallel(num_threads, num_ops));
    if(did_report_success) {
        actual_result = Result ::success;
    }

    EXPECT_EQ(actual_result, expected_result);
}

/**
 * For each item in a vector of input parameters, where either all or none are expected to fail,
 * generate a vector of RunParallelParams such that each test will be run on each device.
 * @param devices
 * @param param_pairs
 * @param expect_success
 * @return
 */
std::vector<RunParallelParam>
permuteRunParallelTests(const std::vector<DeviceParam>& devices,
                        const std::vector<std::pair<size_t, size_t>>& param_pairs,
                        Result expected_result)
{
    std::vector<RunParallelParam> params;

    for(const auto& device : devices) {
        for(const auto& pp : param_pairs) {
            params.emplace_back(device, pp.first, pp.second, expected_result);
        }
    }
    return params;
}

/**
 * List of named, commonly used parameters
 */
namespace run_parallel_params {
const long long small_ops = 1;
const long long large_ops = 1ULL << 22ULL; // 2^22
const int small_threads = 1;
const int med_threads = 64;
const int large_threads = 1u << 14u; // 2^14
} // namespace run_parallel_params

/**
 * Generate a vector of test case parameters for each device that call the RunParallel test with
 * a variety of plausible input parameters.
 * @return
 */
std::vector<RunParallelParam> getRunParallelValidTestParams()
{
    using namespace run_parallel_params;
    auto&& devices = getDevices();
    std::vector<std::pair<size_t, size_t>> valid_param_pairs;

    valid_param_pairs.emplace_back(small_threads, small_ops);
    valid_param_pairs.emplace_back(small_threads, large_ops);

    valid_param_pairs.emplace_back(med_threads, small_ops);
    valid_param_pairs.emplace_back(med_threads, large_ops);

    valid_param_pairs.emplace_back(large_threads, small_ops);
    return permuteRunParallelTests(devices, valid_param_pairs, Result::success);
}

/**
 * Generate a vector of test case parameters that can be used to demonstrate that invalid parameters
 * are properly caught and handled.
 * @return
 */
std::vector<RunParallelParam> getRunParallelInvalidTestParams()
{
    auto&& devices = getDevices();
    std::vector<std::pair<size_t, size_t>> invalid_param_pairs;
    invalid_param_pairs.emplace_back(0, 1);
    return permuteRunParallelTests(devices, invalid_param_pairs, Result::failure);
}

/**
 * Generate a vector of test case parameters that are expected to pass, however may take some time
 * to run.
 * @return
 */
std::vector<RunParallelParam> getRunParallelLongTestParams()
{
    using namespace run_parallel_params;
    auto&& devices = getDevices();
    std::vector<std::pair<size_t, size_t>> long_param_pairs;
    long_param_pairs.emplace_back(large_threads, large_ops);
    return permuteRunParallelTests(devices, long_param_pairs, Result::success);
}

INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_RunParallel_Valid,
                        RunParallelFixture,
                        ::testing::ValuesIn(getRunParallelValidTestParams()),
                        common::PrintParamName());

INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_RunParallel_Invalid,
                        RunParallelFixture,
                        ::testing::ValuesIn(getRunParallelInvalidTestParams()),
                        common::PrintParamName());

INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_RunParallel_LONG,
                        RunParallelFixture,
                        ::testing::ValuesIn(getRunParallelLongTestParams()),
                        common::PrintParamName());

}