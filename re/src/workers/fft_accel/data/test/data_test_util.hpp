//
// Created by Jackson Michael on 26/11/20.
//

#ifndef SEM_FFT_ACCEL_DATA_TEST_UTIL_H
#define SEM_FFT_ACCEL_DATA_TEST_UTIL_H

#include <vector>
#include <random>

#include "data/fft_data.hpp"

namespace sem::fft_accel::data::test {

    template<typename T>
    inline T get_random();

    template<>
    inline float get_random<float>() {
        static std::default_random_engine e;
        static std::uniform_real_distribution<> dis(0, 1); // rage 0 - 1
        return dis(e);
    }

    template<>
    inline uint8_t get_random<uint8_t>() {
        static std::default_random_engine e;
#ifdef _WIN32 // MSVC doesn't allow for uniform_int_distribution of 8-bit types
        static std::uniform_int_distribution<uint16_t> dis(
                std::numeric_limits<uint8_t>::min(),
                std::numeric_limits<uint8_t>::max()
                );
#else
        static std::uniform_int_distribution<uint8_t> dis(
                std::numeric_limits<uint8_t>::min(),
                std::numeric_limits<uint8_t>::max()
                );
#endif
        return dis(e);
    }

    template<>
    inline std::byte get_random<std::byte>() {
        return static_cast<std::byte>(get_random<uint8_t>());
    }

    inline std::vector<float> generate_random_fft_data(size_t num_elements) {
        std::vector<float> fft_data_vector(num_elements);
        for (auto &element : fft_data_vector) {
            element = get_random<float>();
        }
        return fft_data_vector;
    }

    inline std::vector<float> generate_random_single_packet_fft_vec_data() {
        return generate_random_fft_data(fft_data_packet<float>::max_elements);
    }

    template<typename T, size_t NumElements>
    inline std::array<T, NumElements> generate_random_array() {
        std::array<T, NumElements> array;
        for (auto &element : array) {
            element = get_random<T>();
        }
        return array;
    }

    template<typename SampleType>
    inline fft_data_packet <SampleType> generate_random_fft_data_packet() {
        return {
                generate_random_single_packet_fft_vec_data(),
                get_random<uint8_t>(),
                get_random<uint8_t>()
        };
    }

    template<typename SampleType>
    inline serialized_fft_data <SampleType> generate_random_serialized_data_packet() {
        return serialized_fft_data<SampleType>(
                generate_random_array<std::byte, serialized_fft_data<SampleType>::packet_byte_size>()
        );
    }
}

#endif //SEM_FFT_ACCEL_DATA_TEST_UTIL_H
