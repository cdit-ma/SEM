//
// Created by Jackson Michael on 8/9/20.
//

#ifndef SEM_FFT_DATA_HPP
#define SEM_FFT_DATA_HPP

#include <cstddef>
#include <vector>

namespace sem::fft_accel::data {

    struct serialized_fft_data {
        serialized_fft_data(std::byte *data, size_t length) : data_begin(data), byte_length(length) {};
        std::byte *data_begin;
        size_t byte_length;
    };

    template<typename SampleType, typename SerializedType>
    class serializable_fft_data {
    public:
        serializable_fft_data(const std::vector<SampleType>& data);
        serializable_fft_data(const SerializedType& serialized_data);

        SerializedType serialize();
    };

};

#endif //SEM_FFT_DATA_HPP
