//
// Created by Jackson Michael on 10/12/20.
//

#ifndef SEM_FFT_ACCEL_EVENT_DISPATCHER_TESTS_HPP
#define SEM_FFT_ACCEL_EVENT_DISPATCHER_TESTS_HPP

#include "gtest/gtest.h"

#include "runtime/event_dispatcher.hpp"

namespace sem::fft_accel::runtime::test {

    template<typename EventID, typename EventData>
    ::testing::AssertionResult can_register_callback(event_dispatcher <EventID, EventData> &test_dispatcher,
                                                     typename event_dispatcher<EventID, EventData>::Callback callback) {
        try {
            auto register_result = test_dispatcher.register_callback(callback);
            if (register_result.is_error()) {
                return ::testing::AssertionFailure() << "Failed to register callback" << register_result.GetError();
            } else {
                return ::testing::AssertionSuccess();
            }
        } catch (const std::exception &ex) {
            return ::testing::AssertionFailure() << "An exception occurred during register callback" << ex.what();
        }
    }

    template<typename EventID, typename EventData>
    ::testing::AssertionResult can_post_events(event_dispatcher <EventID, EventData> &test_dispatcher,
                                               EventID id, EventData data) {
        try {
            auto submit_result = test_dispatcher.submit_event(id, data);
            if (submit_result.is_error()) {
                return ::testing::AssertionFailure() << "Failed to post data: " << submit_result.GetError();
            } else {
                return ::testing::AssertionSuccess();
            }
        } catch (const std::exception &ex) {
            return ::testing::AssertionFailure() << "An exception occurred during register callback: " << ex.what();
        }
    }

    template<typename EventID, typename EventData>
    ::testing::AssertionResult can_submit_and_receive(event_dispatcher <EventID, EventData> &test_dispatcher,
                                                      typename event_dispatcher<EventID, EventData>::Callback callback,
                                                      EventID id, EventData data) {
        try {
            auto register_result = test_dispatcher.register_callback(callback);
            if (register_result.is_error()) {
                return ::testing::AssertionFailure() << "Failed to register callback" << register_result.GetError();
            }

            auto submit_result = test_dispatcher.submit_event(id, data);
            if (submit_result.is_error()) {
                return ::testing::AssertionFailure() << "Failed to post data" << submit_result.GetError();
            } else {
                return ::testing::AssertionSuccess();
            }
        } catch (const std::exception &ex) {
            return ::testing::AssertionFailure() << "An exception occurred during register callback" << ex.what();
        }
    }
}

#endif //SEM_EVENT_DISPATCHER_TESTS_HPP
