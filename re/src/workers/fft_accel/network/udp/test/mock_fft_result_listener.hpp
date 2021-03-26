//
// Created by Jackson on 26/10/20.
//

#ifndef SEM_MOCK_FFT_RESULT_LISTENER_HPP
#define SEM_MOCK_FFT_RESULT_LISTENER_HPP

#include "gmock/gmock.h"

#include "network/adapter.hpp"

namespace sem::fft_accel::network::test {

    class mock_fft_result_listener : public network::fft_result_listener {
    public:
        MOCK_METHOD((Result < void > ), receive_processed_fft, (std::vector<float> result), (override));
        MOCK_METHOD(bool, test_func, (std::vector<float> result), (override));
    };

}

#endif //SEM_MOCK_FFT_RESULT_LISTENER_HPP
