//
// Created by Jackson Michael on 10/12/20.
//

#ifndef SEM_CALLBACK_HANDLER_HPP
#define SEM_CALLBACK_HANDLER_HPP

#include "gmock/gmock.h"
#include "sem_fft_accel_worker.hpp"

namespace sem::fft_accel::runtime::test {

    class callback_handler {
    public:
        using callback_sig = Worker::callback_func_signature;
        using callback_function_type = Worker::callback_func_type;

        virtual ~callback_handler() = 0;

        virtual callback_sig fft_received_callback = 0;
    };

    inline callback_handler::~callback_handler() = default;

    class mock_callback_handler : public callback_handler {
    public:
        ~mock_callback_handler() override = default;
        MOCK_METHOD(callback_function_type::result_type,
                    fft_received_callback,
                    ((callback_function_type::first_argument_type id), (callback_function_type::second_argument_type data)),
                    (override));
    };
}

#endif //SEM_CALLBACK_HANDLER_HPP
