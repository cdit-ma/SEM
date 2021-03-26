//
// Created by Jackson Michael on 26/8/20.
//

#include "gtest/gtest.h"

#include "runtime/adapter_impl.hpp"

#include "mock_network_adapter.hpp"
#include "data/test/data_test_util.hpp"

#include <deque>

using namespace sem;
using namespace sem::fft_accel;
using namespace sem::fft_accel::test::runtime;

using test_runtime_adapter_type = runtime::adapter_impl;

// For googletest AssertionResult
using namespace testing;

using namespace std::string_literals;

#define FAIL_ON_ERROR(desc, result) if ((result).is_error()) { \
    return ::testing::AssertionFailure() << (desc) << ": " << (result).GetError(); \
    }

TEST(fft_accel_runtime_adapter, nothrow_constructor) {
    std::unique_ptr<runtime::adapter> runtime_adapter;

    ASSERT_NO_THROW(
            runtime_adapter = std::make_unique<test_runtime_adapter_type>();
    );
    ASSERT_NE(runtime_adapter, nullptr);
}

TEST(fft_accel_runtime_adapter, network_adapter_registered_when_set) {

    // Construct runtime adapter under test
    std::unique_ptr<runtime::adapter> runtime_adapter;
    runtime_adapter = std::make_unique<test_runtime_adapter_type>();

    // Construct mocked network adapter and set its expectations
    auto mock_adapter = std::make_shared<mock_network_adapter<float>>();
    ASSERT_NE(mock_adapter, nullptr);
    EXPECT_CALL(*mock_adapter, register_listener);

    // Perform the set
    try {
        ASSERT_EQ(runtime_adapter->set_network_adapter(mock_adapter), Result<void>{});
    } catch (const std::exception &ex) {
        FAIL() << "An exception was thrown when set_network_adapter was called: " << ex.what();
    } catch (...) {
        FAIL() << "A non-std::exception type was thrown when set_network_adapter was called";
    }
}

TEST(fft_accel_runtime_adapter, fft_submitted_successfully) {
    // Construct runtime adapter under test
    std::unique_ptr<runtime::adapter> runtime_adapter;
    runtime_adapter = std::make_unique<test_runtime_adapter_type>();

    // Construct mocked network adapter and set its expectations
    auto mock_adapter = std::make_shared<mock_network_adapter<float>>();
    ASSERT_NE(mock_adapter, nullptr);
    EXPECT_CALL(*mock_adapter, register_listener);
    EXPECT_CALL(*mock_adapter, send);

    std::vector<float> dummy_input_data = data::test::generate_random_single_packet_fft_vec_data();

    ASSERT_FALSE(runtime_adapter->set_network_adapter(mock_adapter).is_error());
    auto result = runtime_adapter->submit_fft_calculation(dummy_input_data);
    ASSERT_FALSE(result.is_error());
}

TEST(fft_accel_runtime_request_map, nothrow_constructor) {
    ASSERT_NO_THROW(runtime::fft_request_map<float>());
}

TEST(fft_accel_runtime_request_map, can_construct_single_packet_request) {
    runtime::fft_request_map<float> request_map;
    using pending_request_type = runtime::pending_request<float>;

    auto input_data = data::test::generate_random_single_packet_fft_vec_data();

    std::unique_ptr<sem::Result<pending_request_type>> result;
    ASSERT_NO_THROW(
            result = std::make_unique<sem::Result<pending_request_type>>(
                    request_map.construct_packets(input_data)
            ));

    std::unique_ptr<pending_request_type> request;
    ASSERT_NO_THROW(request = std::make_unique<pending_request_type>(std::move(result->GetValue())));
    ASSERT_EQ(request->packet_group.packets().size(), 1);
}

TEST(fft_accel_runtime_request_map, can_construct_multipacket_request) {
    runtime::fft_request_map<float> request_map;
    using request_type = runtime::pending_request<float>;

    size_t expected_num_packets = 4;

    size_t vec_length = data::fft_data_packet<float>::max_elements * expected_num_packets;
    auto input_data = data::test::generate_random_fft_data(vec_length);

    std::unique_ptr<sem::Result<request_type>> result;
    ASSERT_NO_THROW(
            result = std::make_unique<sem::Result<request_type>>(
                    request_map.construct_packets(input_data)
            ));

    std::unique_ptr<request_type> request;
    ASSERT_NO_THROW(request = std::make_unique<request_type>(std::move(result->GetValue())));
    ASSERT_EQ(request->packet_group.packets().size(), expected_num_packets);
}

template<typename RequestMap>
AssertionResult map_can_construct_then_receive(size_t num_packets) {
    using request_type = typename RequestMap::fft_request_type;
    using packet_type = typename request_type::packet_type;

    // Assert test preconditions
    if (num_packets == 0) {
        return AssertionFailure() << "Testing packet construction for an FFT map requires a non-zero number of packets";
    }

    // Construct instance of request_map type under test
    RequestMap request_map;

    // Generate test request from random input data
    auto input_data = data::test::generate_random_fft_data(packet_type::max_elements * num_packets);
    auto construct_result = request_map.construct_packets(input_data);
    FAIL_ON_ERROR("An error occurred when constructing packets", construct_result);
    auto&& unfulfilled_req = construct_result.GetValue();

    auto constructed_request_id = unfulfilled_req.packet_group.request_id();

    std::deque<packet_type> response_packets;
    for (size_t sequence_num = 0; sequence_num < num_packets; sequence_num++) {
        // Generate arbitrary data for response packet - we only need the metadata to match, not checking calculation
        // integrity of the actual FFT operation
        response_packets.emplace_back(
                data::vector_range{data::test::generate_random_fft_data(packet_type::max_elements)},
                constructed_request_id,
                sequence_num,
                input_data.size()/2);
    }

    // Send responses for all packets apart from the last, expecting no error, but result should contain nullopt (request not yet fulfilled)
    while (response_packets.size() > 1) {
        packet_type &&current_packet = std::move(response_packets.front());
        auto update_result = request_map.update_pending_packets(current_packet);
        response_packets.pop_front();
        FAIL_ON_ERROR("An error occurred when updating with response for packet with sequence number"s +
                      std::to_string(current_packet.sequence_number()), update_result);
        if (update_result.is_error()) {
            return AssertionFailure()
                    << "An error occurred when updating with response for packet with sequence number "
                    << current_packet.sequence_number() << ": " << update_result.GetError();
        }
    }

    // We expect to receive a completed request after updating with the last packet (no error, no nullopt)
    auto update_result = request_map.update_pending_packets(response_packets.front());
    response_packets.pop_front();
    FAIL_ON_ERROR("An error occurred when updating the final packet", update_result);
    auto fulfilled_request = unfulfilled_req.future.get();

    // The metadata of the request should match
    if (fulfilled_request.request_id() != constructed_request_id) {
        return AssertionFailure() << "ID of fulfilled request was " << fulfilled_request.request_id()
                                  << ", expected " << constructed_request_id;
    }
    if (fulfilled_request.packets().size() != num_packets) {
        return AssertionFailure() << "Number of packets in fulfilled request was " << fulfilled_request.request_id()
                                  << ", expected " << num_packets;
    }
    if (fulfilled_request.remaining_packets() != 0) {
        return AssertionFailure() << "Number of remaining packets in fulfilled request was "
                                  << fulfilled_request.request_id() << ", expected " << 0;
    }

    return AssertionSuccess();
}

TEST(fft_accel_runtime_request_map, can_construct_then_receive_response_single_packet) {
    using request_map_type = runtime::fft_request_map<float>;
    ASSERT_TRUE(map_can_construct_then_receive<request_map_type>(1)); // 256
}

TEST(fft_accel_runtime_request_map, can_construct_then_receive_response_4_packets) {
    using request_map_type = runtime::fft_request_map<float>;
    ASSERT_TRUE(map_can_construct_then_receive<request_map_type>(4)); // 1024
}