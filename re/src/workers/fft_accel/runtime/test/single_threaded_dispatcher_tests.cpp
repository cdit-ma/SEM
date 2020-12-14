//
// Created by Jackson Michael on 10/12/20.
//

#include "gtest/gtest.h"

#include "runtime/single_thread_dispatcher.hpp"

#include "event_dispatcher_tests.hpp"
#include "callback_handler.hpp"
#include "data/test/data_test_util.hpp"

using namespace sem::fft_accel;
using namespace sem::fft_accel::runtime;
using namespace sem::fft_accel::runtime::test;

using namespace std::chrono_literals;

using namespace testing;

using interface_type = event_dispatcher<uint8_t, std::vector<float>>;
using implementation_type = single_thread_dispatcher<uint8_t, std::vector<float>>;

TEST(sem_fft_accel_runtime_single_threaded_dispatcher, construct_and_destruct_dont_throw) {
    std::unique_ptr<interface_type> test_dispatcher;
    ASSERT_NO_THROW(test_dispatcher.reset(new implementation_type()));
    ASSERT_NO_THROW(test_dispatcher.reset());
}

TEST(sem_fft_accel_runtime_single_threaded_dispatcher, can_register_callback) {
    implementation_type dispatcher;
    mock_callback_handler callbackHandler;

    ASSERT_TRUE(can_register_callback(dispatcher, std::bind(&callback_handler::fft_received_callback,
                                                            &callbackHandler,
                                                            std::placeholders::_1, std::placeholders::_2)));
}

TEST(sem_fft_accel_runtime_single_threaded_dispatcher, can_submit_events) {
    implementation_type dispatcher;
    mock_callback_handler callbackHandler;

    auto input_id = data::test::get_random<uint8_t>();
    auto input_data = data::test::generate_random_single_packet_fft_vec_data();

    ASSERT_TRUE(can_post_events(dispatcher, input_id, input_data));
}

ACTION_P(Fulfil, promise) {
    promise.set_value();
}

TEST(sem_fft_accel_runtime_single_threaded_dispatcher, callback_receives_submitted_data) {
    implementation_type dispatcher;

    MockFunction<interface_type::CallbackSignature> mock_callback;

    auto input_id = data::test::get_random<uint8_t>();
    auto input_data = data::test::generate_random_single_packet_fft_vec_data();

    std::promise<void> callback_promise;

    EXPECT_CALL(mock_callback, Call(input_id, input_data))
            .WillOnce(Invoke([&callback_promise](uint8_t, const std::vector<float> &) {
                callback_promise.set_value();
            }));

    ASSERT_TRUE(can_submit_and_receive(dispatcher, mock_callback.AsStdFunction(), input_id, input_data));

    // Wait for a max of 300ms before tearing down (will end immediately once the callback sets the promise)
    callback_promise.get_future().wait_for(300ms);
}