#include "common.h"

struct RunParallelParam {
    RunParallelParam(DeviceParam device, size_t num_threads, size_t num_ops, bool expect_success=true) {
        this->device = device;
        this->num_threads = num_threads;
        this->num_ops = num_ops;
        this->expect_success = expect_success;
    }
    DeviceParam device;
    size_t num_threads;
    size_t num_ops;
    bool expect_success;
};

std::ostream& operator<<(std::ostream& os, const RunParallelParam& rpp) {
    return os << rpp.device << " - threads: [" << rpp.num_threads << "] - operations: [" << rpp.num_ops << "]";
}

class RunParallelFixture:
    public ::testing::TestWithParam<RunParallelParam>,
    public OpenCL_WorkerConstructor {
public:
    RunParallelFixture() : OpenCL_WorkerConstructor(GetParam().device) {
        if(!worker_.Configure()) {
            throw std::runtime_error("Failed to configure worker in RunParallelFixture constructor");
        }
    }
};

TEST_P(RunParallelFixture, RunParallel)
{
    auto num_threads = GetParam().num_threads;
    auto num_ops = GetParam().num_ops;

	//auto start = std::chrono::steady_clock::now();
	EXPECT_TRUE(worker_.RunParallel(num_threads, num_ops));
	//auto end = std::chrono::steady_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
	
}

std::vector<RunParallelParam> getRunParallelTests(
        const std::vector<DeviceParam>& devices,
        const std::vector<std::pair<size_t, size_t> >& param_pairs,
        bool expect_success=true) {

    std::vector<RunParallelParam> params;

    for (const auto& device : devices) {
        for (const auto& pp : param_pairs) {
            params.emplace_back(device, pp.first, pp.second, expect_success);
        }
    }
    return params;
}

std::vector<RunParallelParam> getRunParallelTestList() {
    std::vector<RunParallelParam> all_params;

    long long small_ops = 1;
    long long large_ops = 1LL << 22LL;  // 2^22
    int small_threads = 1;
    int med_threads = 64;
    int large_threads = 1<<14;  // 2^14

    auto&& devices = getDevices();
    std::vector<std::pair<size_t, size_t> > param_pairs;

    param_pairs.emplace_back(small_threads, small_ops);
    param_pairs.emplace_back(small_threads, large_ops);

    param_pairs.emplace_back(med_threads, small_ops);
    param_pairs.emplace_back(med_threads, large_ops);

    param_pairs.emplace_back(large_threads, small_ops);
    param_pairs.emplace_back(large_threads, large_ops);

    return getRunParallelTests(devices, param_pairs, true);
}

INSTANTIATE_TEST_CASE_P(LONG_RunParallel, RunParallelFixture, ::testing::ValuesIn(getRunParallelTestList()));