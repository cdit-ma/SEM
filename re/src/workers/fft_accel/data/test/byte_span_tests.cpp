//
// Created by Jackson Michael on 2/2/21.
//

#include "gtest/gtest.h"

#include "data/byte_span.hpp"

#include "data_test_util.hpp"

#include <optional>

using namespace sem::fft_accel::data;
using namespace sem::fft_accel::data::test;

/*TEST(fft_accel_worker_byte_span, constructor_raw_array_nothrow) {
    std::vector<std::byte> test_vector{8};
    ASSERT_NO_THROW(byte_span test_span(test_vector.begin(), test_vector.size()));
}

TEST(fft_accel_worker_byte_span, constructor_raw_array_empty_nothrow) {
    std::vector<std::byte> test_vector{0};
    ASSERT_NO_THROW(byte_span test_span(test_vector.data(), test_vector.size()));
}*/

TEST(fft_accel_worker_byte_span, constructor_array_nothrow) {
    std::array<std::byte, 8> test_array{};
    ASSERT_NO_THROW(byte_span test_span(test_array));
}

TEST(fft_accel_worker_byte_span, constructor_empty_array_nothrow) {
    std::array<std::byte, 0> test_array_empty{};
    ASSERT_NO_THROW(byte_span test_span(test_array_empty));
}

TEST(fft_accel_worker_byte_span, begin_returns_constructor_input) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span(test_array);
    ASSERT_EQ(test_array.begin(), test_span.begin());
}

TEST(fft_accel_worker_byte_span, end_returns_constructor_input) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span(test_array);
    ASSERT_EQ(test_array.end(), test_span.end());
}

TEST(fft_accel_worker_byte_span, size_returns_constructor_input) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span(test_array);
    ASSERT_EQ(test_array.size(), test_span.size());
}

TEST(fft_accel_worker_byte_span, subscript_correct_for_all_valid_indices) {
    auto test_array = test::generate_random_array<std::byte, 8>();
    byte_span test_span(test_array);
    for (size_t index = 0; index < test_array.size(); index++) {
        ASSERT_EQ(test_array[index], test_span[index]);
    }
}

TEST(fft_accel_worker_byte_span, at_of_byte_size_correct_for_all_valid_indices) {
    auto test_array = test::generate_random_array<std::byte, 8>();
    byte_span test_span(test_array);
    for (size_t index = 0; index < test_array.size(); index++) {
        ASSERT_EQ(test_array.at(index), test_span.at<std::byte>(index));
    }
}

TEST(fft_accel_worker_byte_span, at_of_short_size_correct_for_all_valid_indices) {
    auto test_array = test::generate_random_array<std::byte, 8>();
    byte_span test_span(test_array);
    for (size_t index = 0; index < test_array.size() - 1; index++) {
        auto expected_raw_ptr = &test_array.at(index);
        uint16_t expected_value = *reinterpret_cast<uint16_t *>(expected_raw_ptr);
        ASSERT_EQ(expected_value, test_span.at<uint16_t>(index));
    }
}

TEST(fft_accel_worker_byte_span, at_throws_oor_exception_for_invalid_index) {
    std::array<std::byte, 8> test_array{};
    ASSERT_THROW(auto val = test_array.at(test_array.size()), std::out_of_range);
    ASSERT_THROW(auto val = test_array.at(test_array.size() + 1), std::out_of_range);
    if (test_array.size() < std::numeric_limits<size_t>::max()) {
        ASSERT_THROW(auto val = test_array.at(std::numeric_limits<size_t>::max()), std::out_of_range);
    }
}

TEST(fft_accel_worker_byte_span, at_of_short_size_throws_during_buffer_overrun) {
    auto test_array = test::generate_random_array<std::byte, 8>();
    byte_span test_span(test_array);

    ASSERT_THROW(auto val = test_span.at<uint16_t>(test_span.size() - 1), std::out_of_range);
}

TEST(fft_accel_worker_byte_span, subspan_of_full_length_matches_original_span) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span{test_array};

    std::optional<byte_span> test_subspan;
    ASSERT_NO_THROW(test_subspan = test_span.subspan(0));
    ASSERT_EQ(test_subspan.value().begin(), test_span.begin());
    ASSERT_EQ(test_subspan.value().end(), test_span.end());
}

TEST(fft_accel_worker_byte_span, subspan_throws_if_offset_would_overrun) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span{test_array};

    std::optional<byte_span> test_subspan;
    ASSERT_THROW(test_subspan = test_span.subspan(test_span.size()), std::out_of_range);
}

TEST(fft_accel_worker_byte_span, subspan_of_fixed_length_is_correct) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span{test_array};
    size_t subspan_length = 4;

    std::optional<byte_span> test_subspan;
    ASSERT_NO_THROW(test_subspan = test_span.subspan(0, subspan_length));
    ASSERT_EQ(test_subspan.value().begin(), test_span.begin());
    ASSERT_EQ(test_subspan.value().end(), test_span.begin() + subspan_length);
}

TEST(fft_accel_worker_byte_span, subspan_fixed_length_with_offset_is_correct) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span{test_array};
    size_t subspan_offset = 2;
    size_t subspan_length = 4;

    std::optional<byte_span> test_subspan;
    ASSERT_NO_THROW(test_subspan = test_span.subspan(subspan_offset, subspan_length));
    ASSERT_EQ(test_subspan.value().begin(), test_span.begin() + subspan_offset);
    ASSERT_EQ(test_subspan.value().end(), test_span.begin() + subspan_offset + subspan_length);
}

TEST(fft_accel_worker_byte_span, subspan_fixed_length_throws_if_valid_offset_but_length_would_overrun) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span{test_array};

    ASSERT_THROW(auto test_subspan = test_span.subspan(0, test_array.size() + 1), std::out_of_range);
    ASSERT_THROW(auto test_subspan = test_span.subspan(0, std::numeric_limits<size_t>::max()), std::out_of_range);
}

TEST(fft_accel_worker_byte_span, subspan_fixed_length_throws_if_invalid_offset) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span{test_array};

    ASSERT_THROW(auto test_subspan = test_span.subspan(test_array.size(), 1), std::out_of_range);
}

TEST(fft_accel_worker_byte_span, subspan_would_overrun_despite_valid_offset_and_length) {
    std::array<std::byte, 8> test_array{};
    byte_span test_span{test_array};

    ASSERT_THROW(auto test_subspan = test_span.subspan(test_array.size()-1, 2), std::out_of_range);
}