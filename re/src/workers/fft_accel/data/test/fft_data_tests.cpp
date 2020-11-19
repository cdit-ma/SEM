//
// Created by cdit-ma on 14/11/20.
//

#include "gtest/gtest.h"

#include "data/fft_data.hpp"

using namespace sem::fft_accel::data;

TEST(fft_accel_worker_data, data_packet_constructor_nothrow) {
    std::unique_ptr<fft_data_packet<float>> data_packet;
    ASSERT_NO_THROW(
            data_packet = std::make_unique<fft_data_packet<float>>(std::vector{1.0f,2.0f,3.0f,4.0f},0);
                    );
    ASSERT_NE(data_packet, nullptr);
}

TEST(fft_accel_worker_data, serialized_data_packet_constructor_nothrow) {
    std::array<std::byte, 14> byte_array{/*
                                        static_cast<std::byte>(1), static_cast<std::byte>(2),
                                        static_cast<std::byte>(3), static_cast<std::byte>(4),
                                        static_cast<std::byte>(1), static_cast<std::byte>(2),
                                        static_cast<std::byte>(3), static_cast<std::byte>(4)
                                        */};

    serialized_fft_data<float, 2> test(byte_array);

    std::unique_ptr<serialized_fft_data<float, 2>> serialized_data;
    ASSERT_NO_THROW(
    (serialized_data = std::make_unique<serialized_fft_data<float, (size_t)2>>(byte_array));
    );
    ASSERT_NE(serialized_data, nullptr);
}