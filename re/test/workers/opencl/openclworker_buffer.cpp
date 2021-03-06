#include "common.h"

#include <optional>

using namespace Re::OpenCL;

template<typename T> struct BufferParam {
    BufferParam(DeviceParam device, std::vector<T> data, bool expect_valid = true)
    {
        this->device = device;
        this->data = data;
        this->expect_valid = expect_valid;
    }
    DeviceParam device;
    std::vector<T> data;
    bool expect_valid;
};

template<typename T> std::ostream& operator<<(std::ostream& os, const BufferParam<T>& bp)
{
    return os << bp.device << " - Buffer Type: [" << typeid(T).name() << "] - Buffer Size: [" << bp.data.size() << "]";
}

template<typename T>
class BufferFixture : public ::testing::TestWithParam<BufferParam<T>>, public OpenCL_WorkerConstructor {
    public:
    BufferFixture() : OpenCL_WorkerConstructor(::testing::TestWithParam<BufferParam<T>>::GetParam().device)
    {
        if(!worker_.Configure()) {
            throw std::runtime_error("Failed to configure worker in BufferFixture constructor");
        }
    }
};

// Typedef the types of buffers we wish to test
typedef BufferFixture<float> FloatFixture;
typedef BufferFixture<int> IntFixture;
typedef BufferFixture<double> DoubleFixture;

template<typename T> void BufferTest(OpenCL_Worker& worker_, const BufferParam<T> bf)
{
    // Create buffer
    std::optional<Buffer<T>> buffer;
    ASSERT_NO_THROW(buffer = std::move(worker_.CreateBuffer(bf.data, true)));

    // Check for valid buffer
    auto is_valid = buffer.value().IsValid();
    ASSERT_EQ(is_valid, bf.expect_valid);
    ASSERT_EQ(worker_.IsBufferValid(buffer.value()), bf.expect_valid);

    // Check the buffers values
    std::vector<T> read_buffer;
    ASSERT_NO_THROW(read_buffer = worker_.ReadBuffer(buffer.value(), true));
    if(is_valid) {
        ASSERT_EQ(read_buffer, bf.data);
    } else {
        std::cout << "test" << std::endl;
        std::vector<T> empty_vec;
        std::cout << read_buffer.size() << std::endl;
        ASSERT_EQ(read_buffer, empty_vec);
    }

    // Release the buffer
    ASSERT_NO_THROW(worker_.ReleaseBuffer(buffer.value()));
    ASSERT_FALSE(worker_.IsBufferValid(buffer.value()));
}

TEST_P(FloatFixture, Test) { BufferTest(worker_, GetParam()); }

TEST_P(IntFixture, Test) { BufferTest(worker_, GetParam()); }

TEST_P(DoubleFixture, Test) { BufferTest(worker_, GetParam()); }

template<typename T>
std::vector<BufferParam<T>>
getBufferTests(std::vector<DeviceParam> devices, std::vector<std::vector<T>> buffer_values, bool expect_valid)
{
    std::vector<BufferParam<T>> params;

    for(const auto& device : devices) {
        for(const auto& buffer_value : buffer_values) {
            params.emplace_back(BufferParam<T>(device, buffer_value, expect_valid));
        }
    }
    return params;
}

template<typename T> std::vector<BufferParam<T>> getBufferTestsLists()
{
    std::vector<BufferParam<T>> all_params;

    auto devices = getDevices();

    std::vector<std::vector<T>> valid_tests;
    std::vector<std::vector<T>> invalid_tests;

    std::vector<T> temp_vector;
    for(int i = 1; i <= 10; i++) {
        temp_vector.push_back(static_cast<T>(i));
        valid_tests.push_back(temp_vector);
    }

    // Test large Vector
    valid_tests.emplace_back(32000000, (T)1);

    // Test Empty
    invalid_tests.emplace_back(std::vector<T>());

    auto valid_params = getBufferTests(devices, valid_tests, true);
    auto invalid_params = getBufferTests(devices, invalid_tests, false);

    all_params.insert(all_params.end(), valid_params.begin(), valid_params.end());
    all_params.insert(all_params.end(), invalid_params.begin(), invalid_params.end());
    return all_params;
}

// Instantiate test cases for the types we require
INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Buffer_Float,
                        FloatFixture,
                        ::testing::ValuesIn(getBufferTestsLists<float>()));
INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Buffer_Int, IntFixture, ::testing::ValuesIn(getBufferTestsLists<int>()));
INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Buffer_Double,
                        DoubleFixture,
                        ::testing::ValuesIn(getBufferTestsLists<double>()));
