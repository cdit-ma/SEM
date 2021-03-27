//
// Created by Jackson Michael on 27/3/21.
//

#include "netdata/chart_json_parser.hpp"

#include "mock_chart_listener.h"

#include "gtest/gtest.h"

using namespace sem::logging::netdata;
using testing::Truly;

namespace sem::logging::netdata::test {
    template<protobuf::chart::chart_type_enum ChartType>
    bool is_netdata_chart_type(std::shared_ptr<const protobuf::chart> chart) {
        return chart->chart_type() == ChartType;
    }
};

TEST(chart_json_parser, default_constructor_doesnt_throw) {
    std::unique_ptr<chart_json_parser> test_parser;

    ASSERT_NO_THROW(
            test_parser = std::make_unique<chart_json_parser>()
    );
}

TEST(chart_json_parser, can_register_chart_listener) {
    chart_json_parser test_parser;

    std::shared_ptr<chart_listener> mock_listener = std::make_shared<test::mock_chart_listener>();

    ASSERT_NO_THROW(
            test_parser.register_listener(mock_listener, protobuf::chart::nvidia_smi)
    );
}

TEST(chart_json_parser, can_send_chart_to_listener) {
    chart_json_parser test_parser;
    auto mock_listener = std::make_shared<test::mock_chart_listener>();

    EXPECT_CALL(*mock_listener, receive_chart_data(
            Truly(&test::is_netdata_chart_type<protobuf::chart::nvidia_smi>)
    ));

    ASSERT_NO_THROW(
            test_parser.register_listener(mock_listener, protobuf::chart::nvidia_smi)
    );

    ASSERT_NO_THROW(
            test_parser.receive_line(R"({"chart_type": "nvidia_smi"})")
    );
}

TEST(chart_json_parser, can_receive_when_no_listeners_registered) {
    chart_json_parser test_parser;

    ASSERT_NO_THROW(
            test_parser.receive_line(R"({"chart_type": "nvidia_smi"})")
    );
}

TEST(chart_json_parser, doesnt_send_mangled_json_to_listener) {
    chart_json_parser test_parser;
    auto mock_listener = std::make_shared<test::mock_chart_listener>();

    // Registering a chart listener that is not expecting any calls
    ASSERT_NO_THROW(
            test_parser.register_listener(mock_listener, protobuf::chart::nvidia_smi)
    );

    ASSERT_NO_THROW(
            test_parser.receive_line(R"({"chart_type"}{"nvidia_smi"})")
    );
}

TEST(chart_json_parser, doesnt_send_invalid_chart_type_to_listener) {
    chart_json_parser test_parser;
    auto mock_listener = std::make_shared<test::mock_chart_listener>();

    // Registering a chart listener that is not expecting any calls
    ASSERT_NO_THROW(
            test_parser.register_listener(mock_listener, protobuf::chart::nvidia_smi)
    );

    ASSERT_NO_THROW(
            test_parser.receive_line(R"({"chart_type": "this_chart_type_doesn't_exist"})")
    );
}

TEST(chart_json_parser, doesnt_send_chart_to_listener_registered_with_different_type) {
    chart_json_parser test_parser;
    auto mock_listener = std::make_shared<test::mock_chart_listener>();

    // Registering a chart listener that is not expecting any calls
    ASSERT_NO_THROW(
            test_parser.register_listener(mock_listener, protobuf::chart::nvidia_smi)
    );

    ASSERT_NO_THROW(
            test_parser.receive_line(R"({"chart_type": "netdata"})")
    );
}

TEST(chart_json_parser, can_send_chart_to_multiple_listeners) {
    chart_json_parser test_parser;
    auto mock_listeners = std::vector
            {{
                     std::make_shared<test::mock_chart_listener>(),
                     std::make_shared<test::mock_chart_listener>(),
             }};

    for (auto &mock_listener : mock_listeners) {
        EXPECT_CALL(*mock_listener, receive_chart_data(
                Truly(&test::is_netdata_chart_type<protobuf::chart::nvidia_smi>)
        ));
    }

    for (auto &mock_listener : mock_listeners) {
        ASSERT_NO_THROW(
                test_parser.register_listener(mock_listener, protobuf::chart::nvidia_smi)
        );
    }

    ASSERT_NO_THROW(
            test_parser.receive_line(R"({"chart_type": "nvidia_smi"})")
    );
}

TEST(chart_json_parser, can_send_to_multiple_listeners_if_one_throws) {
    chart_json_parser test_parser;
    auto mock_listeners = std::vector
            {{
                     std::make_shared<test::mock_chart_listener>(),
                     std::make_shared<test::mock_chart_listener>(),
             }};

    // Simulate the situation in which one of the callbacks will throw an exception
    EXPECT_CALL(*mock_listeners[0], receive_chart_data(
            Truly(&test::is_netdata_chart_type<protobuf::chart::nvidia_smi>)
    )).WillOnce(::testing::Throw(std::runtime_error("Test exception thrown to verify chart_json_parser robustness")));

    // Expect that the other callback is still able to run as it normally would
    EXPECT_CALL(*mock_listeners[1], receive_chart_data(
            Truly(&test::is_netdata_chart_type<protobuf::chart::nvidia_smi>)
    ));

    for (auto &mock_listener : mock_listeners) {
        ASSERT_NO_THROW(test_parser.register_listener(mock_listener, protobuf::chart::nvidia_smi));
    }

    ASSERT_NO_THROW(
            test_parser.receive_line(R"({"chart_type": "nvidia_smi"})")
    );
}