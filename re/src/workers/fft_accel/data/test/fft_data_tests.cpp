//
// Created by Jackson Michael on 14/11/20.
//

#include "gtest/gtest.h"

#include "data/fft_data.hpp"

#include "data_test_util.hpp"

using namespace sem::fft_accel::data;
using namespace sem::fft_accel::data::test;

TEST(fft_accel_worker_data_packet, constructor_nothrow) {
    auto payload_data = generate_random_single_packet_fft_vec_data();
    vector_range<float> payload_range(payload_data, 0, payload_data.size());

    std::unique_ptr<fft_data_packet<float>> data_packet;
    ASSERT_NO_THROW(
            data_packet = std::make_unique<fft_data_packet<float>>(
                    payload_range,
                    get_random<uint8_t>(),
                    get_random<uint8_t>()
            );
    );
    ASSERT_NE(data_packet, nullptr);
}

TEST(fft_accel_worker_serialized_data_packet, constructor_nothrow) {
    constexpr auto num_elems = fft_data_packet<float>::max_elements;
    constexpr auto array_length = serialized_fft_data<float, num_elems>::byte_size_from_num_elements(num_elems);

    std::array<std::byte, array_length> byte_array{/*
                                        static_cast<std::byte>(1), static_cast<std::byte>(2),
                                        static_cast<std::byte>(3), static_cast<std::byte>(4),
                                        static_cast<std::byte>(1), static_cast<std::byte>(2),
                                        static_cast<std::byte>(3), static_cast<std::byte>(4)
                                        */};

    serialized_fft_data<float, num_elems> test(byte_array);

    std::unique_ptr<serialized_fft_data<float, num_elems>> serialized_data;
    ASSERT_NO_THROW(
            (serialized_data = std::make_unique<serialized_fft_data<float, num_elems>>(byte_array));
    );
    ASSERT_NE(serialized_data, nullptr);
}

TEST(fft_accel_worker_serialized_data_packet, serialize_then_deserialize_preserves_data) {
    auto packet = generate_random_fft_data_packet<float>();

    serialized_fft_data<float> serialized_packet(packet);

    auto deserialized_packet = serialized_packet.deserialize();

    ASSERT_EQ(packet.request_id(), deserialized_packet.request_id());
    ASSERT_EQ(packet.sequence_number(), deserialized_packet.sequence_number());
    /*
    // Uncomment below lines for more in-depth diagnosis of data mismatch at specific indices
    for (int i=0; i < packet.payload_data().size(); i++) {
        EXPECT_EQ(packet.payload_data().at(i), deserialized_packet.payload_data().at(i)) << "index value is: " << i;
    }*/
    ASSERT_EQ(packet.payload_data(), deserialized_packet.payload_data());
}