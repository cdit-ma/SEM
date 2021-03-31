//
// Created by Jackson Michael on 27/3/21.
//

#include "gtest/gtest.h"

#include "netdata/device_sample_aggregator.hpp"

#include <chrono>

using namespace sem::logging::netdata;

namespace sem::logging::netdata::test {
    std::unique_ptr<const protobuf::chart> generate_nvidia_smi_with_context(const std::string &context, int64_t value) {
        auto new_msg = std::make_unique<protobuf::chart>();

        new_msg->set_hostname("test_hostname");
        new_msg->set_chart_type(protobuf::chart::nvidia_smi);
        new_msg->set_chart_context(context);
        new_msg->set_value(value);
        new_msg->set_timestamp(std::chrono::system_clock::now().time_since_epoch().count());
        return new_msg;
    }
}

TEST(device_sample_aggregator, default_constructor_doesnt_throw) {
    std::unique_ptr<device_sample_aggregator> test_aggregator;

    ASSERT_NO_THROW(
            test_aggregator = std::make_unique<device_sample_aggregator>();
    );
}

TEST(device_sample_aggregator, can_receive_valid_chart_without_throwing) {
    device_sample_aggregator test_aggregator;

    ASSERT_NO_THROW(
            test_aggregator.receive_chart_data(
                    test::generate_nvidia_smi_with_context("nvidia_smi.gpu_utilization", 1)
            )
    );
}

TEST(device_sample_aggregator, discard_unrecognised_nvidia_smi_chart_context) {
    device_sample_aggregator test_aggregator;

    ASSERT_NO_THROW(
            test_aggregator.receive_chart_data(
                    test::generate_nvidia_smi_with_context("nvidia_smi.invalid_chart_context", 1)
            )
    );

    ASSERT_EQ(test_aggregator.retrieve_device_metrics("test_hostname")->gpu_samples_size(),
              0);
}

TEST(device_sample_aggregator, throw_on_invallid_chart_type) {
    device_sample_aggregator test_aggregator;
    auto invalid_chart_msg = std::make_unique<protobuf::chart>();
    invalid_chart_msg->set_chart_type(protobuf::chart::netdata);

    ASSERT_THROW(
            test_aggregator.receive_chart_data(std::move(invalid_chart_msg)),
            std::invalid_argument
    );

    ASSERT_EQ(test_aggregator.retrieve_device_metrics("test_hostname")->gpu_samples_size(),
              0);
}

TEST(device_sample_aggregator, can_receive_many_charts) {
    device_sample_aggregator test_aggregator;
    const size_t num_test_messages = 32;
    const int64_t test_value = 37;

    for (size_t test_message_count = 0; test_message_count < num_test_messages; test_message_count++) {
        ASSERT_NO_THROW(
                test_aggregator.receive_chart_data(
                        test::generate_nvidia_smi_with_context("nvidia_smi.gpu_utilization", test_value)
                );
        );
    }
    std::unique_ptr<SystemEvent::DeviceMetricSamples> device_samples;
    ASSERT_NO_THROW(device_samples = test_aggregator.retrieve_device_metrics("test_hostname"));

    ASSERT_EQ(device_samples->gpu_samples().size(), num_test_messages);
    for (const auto &sample : device_samples->gpu_samples()) {
        ASSERT_EQ(sample.processor_utilisation_perc(), test_value/100.0f);
    }
}

TEST(device_sample_aggregator, can_receive_gpu_utilization) {
    device_sample_aggregator test_aggregator;
    const int64_t test_value = 54;

    ASSERT_NO_THROW(
            test_aggregator.receive_chart_data(
                    test::generate_nvidia_smi_with_context("nvidia_smi.gpu_utilization", test_value)
            )
    );
    std::unique_ptr<SystemEvent::DeviceMetricSamples> device_samples;
    ASSERT_NO_THROW(device_samples = test_aggregator.retrieve_device_metrics("test_hostname"));

    ASSERT_EQ(device_samples->gpu_samples().size(), 1);
    ASSERT_EQ(device_samples->gpu_samples()[0].processor_utilisation_perc(), test_value/100.0f);
}

TEST(device_sample_aggregator, can_receive_mem_utilization) {
    device_sample_aggregator test_aggregator;
    const int64_t test_value = 63;

    ASSERT_NO_THROW(
            test_aggregator.receive_chart_data(
                    test::generate_nvidia_smi_with_context("nvidia_smi.mem_utilization", test_value)
            )
    );
    std::unique_ptr<SystemEvent::DeviceMetricSamples> device_samples;
    ASSERT_NO_THROW(device_samples = test_aggregator.retrieve_device_metrics("test_hostname"));

    ASSERT_EQ(device_samples->gpu_samples().size(), 1);
    ASSERT_EQ(device_samples->gpu_samples()[0].memory_utilisation_perc(), test_value/100.0f);
}

TEST(device_sample_aggregator, can_receive_temperature) {
    device_sample_aggregator test_aggregator;
    const int64_t test_value = 37;

    ASSERT_NO_THROW(
            test_aggregator.receive_chart_data(
                    test::generate_nvidia_smi_with_context("nvidia_smi.temperature", test_value)
            )
    );
    std::unique_ptr<SystemEvent::DeviceMetricSamples> device_samples;
    ASSERT_NO_THROW(device_samples = test_aggregator.retrieve_device_metrics("test_hostname"));

    ASSERT_EQ(device_samples->gpu_samples().size(), 1);
    ASSERT_EQ(device_samples->gpu_samples()[0].temperature_cel(), test_value);
}