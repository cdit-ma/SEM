#include "common.h"
#include <workers/opencl/opencl_worker.h>

#include <math.h>
#include <random>

#define PI 3.1415926535897932384
#define EPS 1e-6

/****
 * FFT testing
 ****/

using namespace cditma::test::helpers;

struct FFTParam {
    FFTParam(DeviceParam device,
             const std::vector<float>& data_in,
             const std::vector<float>& data_out,
             bool expect_success = true) :
        device_(device),
        data_in_(data_in),
        data_out_(data_out),
        expect_success_(expect_success){};

    DeviceParam device_;
    std::vector<float> data_in_;
    std::vector<float> data_out_;
    bool expect_success_ = true;
};

void addFrequency(std::vector<float>& data, float frequency, float amplitude, float phase_shift)
{
    size_t num_elements = data.size() / 2;
    for(unsigned int i = 0; i < num_elements; i++) {
        data[i * 2] +=
            amplitude
            * (float)(cos((float)frequency * ((float)i / num_elements) * 2 * PI + phase_shift));
    }
}

/**
 * Constant input data should generate a single spike at the 0 bin equal to the length of the vector
 * multiplied by the amplitude
 */
std::vector<float> generateConstantInput(size_t length, float amplitude)
{
    std::vector<float> data(length * 2, 0);
    for(unsigned int i = 0; i < data.size() / 2; i++) {
        data[i * 2] = amplitude;
    }
    return data;
}
std::vector<float> generateConstantOutput(size_t length, float amplitude)
{
    std::vector<float> data(length * 2, 0);
    data[0] = length * amplitude;
    return data;
}

std::vector<float>
generateAlignedFrequencyInput(size_t length, float amplitude, float frequency, size_t phase_shift)
{
    std::vector<float> data(length * 2, 0);
    addFrequency(data, frequency, amplitude, (float)phase_shift);
    return data;
}

std::vector<float>
generateAlignedFrequencyOutput(size_t length, float amplitude, float frequency, size_t phase_shift)
{
    std::vector<float> data(length * 2, (float)0);
    data[(size_t)abs(frequency) * 2] += amplitude * (float)(length / 2) * (float)cos(phase_shift);
    data[(size_t)abs(frequency) * 2 + 1] += amplitude * (float)(length / 2)
                                            * (float)sin(phase_shift);
    data[length * 2 - (size_t)abs(frequency) * 2] += amplitude * (float)(length / 2)
                                                     * (float)cos(phase_shift);
    data[length * 2 - (size_t)abs(frequency) * 2 + 1] += amplitude * (float)(length / 2)
                                                         * (float)sin(phase_shift);
    return data;
}

std::vector<float>
generateMultipleAlignedFrequencyInput(size_t length, float frequency1, float frequency2)
{
    std::vector<float> data(length * 2, 0);
    addFrequency(data, frequency1, 1, 0);
    addFrequency(data, frequency2, 3, 0);
    return data;
}

std::vector<float>
generateMultipleAlignedFrequencyOutput(size_t length, float frequency1, float frequency2)
{
    std::vector<float> data(length * 2, (float)0);
    data[(size_t)abs(frequency1) * 2] += 1 * (float)(length / 2);
    data[(size_t)abs(frequency2) * 2] += 3 * (float)(length / 2);

    data[length * 2 - (size_t)abs(frequency1) * 2] += 1 * (float)(length / 2);
    data[length * 2 - (size_t)abs(frequency2) * 2] += 3 * (float)(length / 2);
    return data;
}

std::ostream& operator<<(std::ostream& os, const FFTParam& f)
{
    return os << f.device_ << ", length: " << f.data_in_.size() / 2
              << " - input data: " << ::testing::PrintToString(f.data_in_)
              << " , output data: " << ::testing::PrintToString(f.data_out_);
};

class FFTFixture : public ::testing::TestWithParam<FFTParam>, public OpenCL_WorkerConstructor {
public:
    FFTFixture() : OpenCL_WorkerConstructor(GetParam().device_)
    {
        if(!worker_.Configure()) {
            throw std::runtime_error("Failed to configure worker in FFTFixture constructor");
        }
    }
};

TEST_P(FFTFixture, FFTtest)
{
    auto data = GetParam().data_in_;
    auto& expected_output = GetParam().data_out_;
    auto expect_success = GetParam().expect_success_;

    // Make sure that test case params are valid in terms of size
    ASSERT_EQ(data.size(), expected_output.size());

    bool did_succeed = false;
    EXPECT_NO_THROW(did_succeed = worker_.FFT(data));
    ASSERT_EQ(did_succeed, expect_success);

    ASSERT_EQ(data.size(), expected_output.size());

    expect_nearly_equal(data, expected_output, 1e-3);
}

typedef std::tuple<std::vector<float>, std::vector<float>, bool> TestData;

/**
 * Permutes all test cases across all devices
 */
std::vector<FFTParam>
permuteFFTTests(std::vector<DeviceParam> devices, std::vector<TestData> testcases)
{
    std::vector<FFTParam> permuted_params;
    for(auto& device : devices) {
        for(auto& testcase : testcases) {
            permuted_params.emplace_back(device, std::get<0>(testcase), std::get<1>(testcase),
                                         std::get<2>(testcase));
        }
    }
    return permuted_params;
}

std::vector<FFTParam> getConstantTests()
{
    std::vector<FFTParam> params;
    std::vector<TestData> tests;
    tests.emplace_back(generateConstantInput(4, 1.0), generateConstantOutput(4, 1.0), true);
    tests.emplace_back(generateConstantInput(4, 7.0), generateConstantOutput(4, 7.0), true);
    tests.emplace_back(generateConstantInput(8, 1.0), generateConstantOutput(8, 1.0), true);
    tests.emplace_back(generateConstantInput(8, 7.0), generateConstantOutput(8, 7.0), true);
    tests.emplace_back(generateConstantInput(8, -7.0), generateConstantOutput(8, 7.0), true);
    tests.emplace_back(generateConstantInput(4096, 1.0), generateConstantOutput(4096, 1.0), true);
    tests.emplace_back(generateConstantInput(4096, 7.0), generateConstantOutput(4096, 7.0), true);
    // Test a non-divisible by two length RE-343
    tests.emplace_back(std::vector<float>(11), std::vector<float>(11), false);
    return permuteFFTTests(getDevices(), tests);
}

std::vector<FFTParam> getSingleAlignedTests()
{
    std::vector<FFTParam> params;
    std::vector<TestData> tests;
    tests.emplace_back(generateAlignedFrequencyInput(4, 1, 1, 0),
                       generateAlignedFrequencyOutput(4, 1, 1, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(4, 2, 1, 0),
                       generateAlignedFrequencyOutput(4, 2, 1, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(4, 2, 2, 0),
                       generateAlignedFrequencyOutput(4, 2, 2, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(4, 1, 1, 1),
                       generateAlignedFrequencyOutput(4, 1, 1, 1), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, 1, 1, 0),
                       generateAlignedFrequencyOutput(16, 1, 1, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, 2, 3, 0),
                       generateAlignedFrequencyOutput(16, 2, 3, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, -2, 3, 0),
                       generateAlignedFrequencyOutput(16, -2, 3, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, 2, -3, 0),
                       generateAlignedFrequencyOutput(16, 2, 3, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(4096, 1, 1, 0),
                       generateAlignedFrequencyOutput(4096, 1, 1, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(4096, 2, 11, 0),
                       generateAlignedFrequencyOutput(4096, 2, 11, 0), true);
    return permuteFFTTests(getDevices(), tests);
}

std::vector<FFTParam> getMultipleAlignedTests()
{
    std::vector<FFTParam> params;
    std::vector<TestData> tests;
    tests.emplace_back(generateMultipleAlignedFrequencyInput(16, 1, 2),
                       generateMultipleAlignedFrequencyOutput(16, 1, 2), true);
    tests.emplace_back(generateMultipleAlignedFrequencyInput(32, 1, 3),
                       generateMultipleAlignedFrequencyOutput(32, 1, 3), true);
    tests.emplace_back(generateMultipleAlignedFrequencyInput(64, 5, -7),
                       generateMultipleAlignedFrequencyOutput(64, 5, -7), true);
    tests.emplace_back(generateMultipleAlignedFrequencyInput(4096, 13, -29),
                       generateMultipleAlignedFrequencyOutput(4096, 13, -29), true);
    return permuteFFTTests(getDevices(), tests);
}

std::vector<FFTParam> getUnalignedTests()
{
    std::vector<FFTParam> params;
    std::vector<TestData> tests;
    tests.emplace_back(generateAlignedFrequencyInput(16, 1, 2.5, 0),
                       generateAlignedFrequencyOutput(16, 1, 2.5, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, 1, 1, 0),
                       generateAlignedFrequencyOutput(16, 1, 1, 0), true);
    return permuteFFTTests(getDevices(), tests);
}

INSTANTIATE_TEST_CASE_P(Re_Workers_OpenclWorker_Constant,
                        FFTFixture,
                        ::testing::ValuesIn(getConstantTests()));
INSTANTIATE_TEST_CASE_P(Re_Workers_OpenclWorker_SingleAlignedFrequency,
                        FFTFixture,
                        ::testing::ValuesIn(getSingleAlignedTests()));
INSTANTIATE_TEST_CASE_P(Re_Workers_OpenclWorker_MultipleAlignedFrequency,
                        FFTFixture,
                        ::testing::ValuesIn(getMultipleAlignedTests()));
// INSTANTIATE_TEST_CASE_P(DISABLED_Re_Workers_OpenclWorker_UnalignedFrequencies, FFTFixture,
// ::testing::ValuesIn(getUnalignedTests()));