//
// Created by cdit-ma on 23/05/19.
//
#include "gtest/gtest.h"
#include <future>
#include <util/execution.hpp>

TEST(ReCommon_Execution, StartInterrupt)
{
    using namespace std;
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    Execution exe;

    auto interrupter = async(launch::async, [&exe] {
        sleep_for(50ms);
        exe.Interrupt();
    });

    exe.Start();
}

TEST(ReCommon_Execution, StartError)
{
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    Execution exe;
    std::string exception_message("TEST_LOGIC_ERROR");

    auto error_terminate = [&exe, &exception_message] {
        throw std::logic_error(exception_message);
    };

    exe.AddTerminateCallback(error_terminate);
    auto interrupter = async(std::launch::async, [&exe, &exception_message] {
        sleep_for(50ms);
        exe.Interrupt();
    });

    try {
        exe.Start();
    } catch(const std::exception& ex) {
        EXPECT_EQ(ex.what(), exception_message + '\n');
    }
}

TEST(ReCommon_Execution, StartMultipleError)
{
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    Execution exe;
    std::string exception_message("TEST_LOGIC_ERROR");
    std::ostringstream expected_stream;
    expected_stream << exception_message << std::endl << exception_message << std::endl;
    std::string expected_output(expected_stream.str());

    // If we have two registered terminate callbacks, we should call them both, regardless of
    // exceptions raised by prior called callbacks
    bool callback_flag1 = false;
    bool callback_flag2 = false;
    auto error_terminate1 = [&exe, &exception_message, &callback_flag1] {
        callback_flag1 = true;
        throw std::logic_error(exception_message);
    };
    auto error_terminate2 = [&exe, &exception_message, &callback_flag2] {
        callback_flag2 = true;
        throw std::logic_error(exception_message);
    };

    exe.AddTerminateCallback(error_terminate1);
    exe.AddTerminateCallback(error_terminate2);

    auto interrupter = std::async(std::launch::async, [&exe, &exception_message] {
        sleep_for(50ms);
        exe.Interrupt();
    });

    try {
        exe.Start();
    } catch(const std::exception& ex) {
        EXPECT_EQ(ex.what(), expected_output);
    }

    EXPECT_TRUE(callback_flag1);
    EXPECT_TRUE(callback_flag2);
}

TEST(ReCommon_Execution, AddTerminateCallback)
{
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    Execution exe;
    bool callback_called = false;
    auto term_callback = [&callback_called] { callback_called = true; };
    exe.AddTerminateCallback(term_callback);

    // Callback should not be called after addition
    EXPECT_FALSE(callback_called);

    auto interrupter = async(std::launch::async, [&exe] {
        sleep_for(50ms);
        exe.Interrupt();
    });

    // Callback should not be called after starting async func that has ref to exe
    EXPECT_FALSE(callback_called);

    exe.Start();

    // Callback should be called after start has been interrupted and completed execution
    EXPECT_TRUE(callback_called);
}

