//
// Created by Jackson Michael on 26/8/20.
//

#include "gtest/gtest.h"

#include "runtime/adapter_impl.hpp"

#include "mock_network_adapter.hpp"

#include "component.h"

using namespace sem;
using namespace sem::fft_accel;
using namespace sem::fft_accel::test::worker;

using test_worker_type = runtime::adapter_impl;

TEST(fft_accel_runtime_adapter, nothrow_constructor) {
    std::unique_ptr<runtime::adapter> runtime_adapter;

    ASSERT_NO_THROW(
            runtime_adapter = std::make_unique<test_worker_type>();
            );
    ASSERT_NE(runtime_adapter, nullptr);
}

TEST(fft_accel_runtime_adapter, network_adapter_registered_when_set) {

    // Construct worker under test
    std::unique_ptr<runtime::adapter> worker;
    worker = std::make_unique<test_worker_type>();

    // Construct mocked network adapter and set its expectations
    auto mock_adapter = std::make_shared<mock_network_adapter<float>>();
    ASSERT_NE(mock_adapter, nullptr);
    EXPECT_CALL(*mock_adapter, register_listener);

    // Perform the set
    try {
        ASSERT_EQ(worker->set_network_adapter(mock_adapter), Result<void>{});
    } catch (const std::exception& ex) {
        FAIL() << "An exception was thrown when set_network_adapter was called: " << ex.what();
    } catch (...) {
        FAIL() << "A non-std::exception type was thrown when set_network_adapter was called";
    }
}

TEST(fft_accel_runtime_adapter, fft_submitted_successfully) {
    // Construct worker under test
    std::unique_ptr<runtime::adapter> worker;
    worker = std::make_unique<test_worker_type>();

    // Construct mocked network adapter and set its expectations
    auto mock_adapter = std::make_shared<mock_network_adapter<float>>();
    ASSERT_NE(mock_adapter, nullptr);
    EXPECT_CALL(*mock_adapter, register_listener);
    EXPECT_CALL(*mock_adapter, send);

    std::vector<float> dummy_input_data{1.0f, 2.0f, 3.0f, 4.0f};

    ASSERT_FALSE(worker->set_network_adapter(mock_adapter).is_error());
    ASSERT_FALSE(worker->submit_fft_calculation(dummy_input_data).is_error());

}