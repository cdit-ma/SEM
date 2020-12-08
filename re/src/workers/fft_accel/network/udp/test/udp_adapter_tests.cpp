//
// Created by Jackson Michael on 3/9/20.
//

#include "gtest/gtest.h"

#include "network/udp/udp_adapter.hpp"
#include "async_fft_result_listener.hpp"

#include "data/test/data_test_util.hpp"

#include <thread>

#include <iostream>

using namespace re::types;
using namespace sem::fft_accel;
using namespace sem::fft_accel::network;
using namespace boost::asio;

using namespace testing;


TEST(fft_accel_udp_adapter, nothrow_constructor) {
    auto test_addr = re::types::SocketAddress("123.234.123.234:12345");
    ASSERT_NO_THROW(udp_adapter<float> test_adapter(test_addr););
}

TEST(fft_accel_udp_adapter, send_single_packet_over_udp) {
    // Create test listener service with a socket bound to some free port
    io_service test_io_service;
    ip::udp::socket listen_socket(test_io_service);
    try {
        listen_socket.open(ip::udp::v4());
        listen_socket.bind(ip::udp::endpoint(ip::address_v4::loopback(), 0));
    } catch (const std::exception &ex) {
        FAIL() << "Failed to bind test udp port with the following exception message: " << ex.what();
    }
    auto bind_port = listen_socket.local_endpoint().port();

    // Spawn a thread to listen for packets and run the service
    auto listen_future =
            std::async(std::launch::async,
                       [&test_io_service, &listen_socket]() -> sem::Result<data::serialized_fft_data<float>> {
                           data::serialized_fft_data<float> listen_data;
                           auto buffer = boost::asio::buffer(listen_data.bytes().begin(), listen_data.bytes().size());
                           listen_socket.receive(buffer);

                           test_io_service.run();
                           return listen_data;
                       });

    udp_adapter<float> test_adapter(SocketAddress(Ipv4::localhost(), bind_port));
    auto input_data = data::test::generate_random_serialized_data_packet<float>();

    auto send_result = test_adapter.send(input_data);
    send_result.GetValue();

    auto result = listen_future.get();
    ASSERT_FALSE(result.is_error());

    auto &&output_data = result.GetValue();
    ASSERT_EQ(output_data.bytes().size(), input_data.bytes().size());
    for (size_t index = 0; index < output_data.bytes().size(); index++) {
        EXPECT_EQ(output_data.bytes()[index], input_data.bytes()[index]) << "Index is " << index;
    }

}

TEST(fft_accel_udp_adapter, receive_multiple_packets_over_udp) {
    // Create test listener so that we can check if a result listener is properly stimulated
    auto mock_listener = std::make_shared<async_fft_result_listener>();

    // Create UDP socket to be used for sending test datagrams
    io_context test_io_context;
    ip::udp::socket send_socket(test_io_context);
    try {
        send_socket.open(ip::udp::v4());
    } catch (const std::exception &ex) {
        FAIL() << "Failed to open test udp port with the following exception message: " << ex.what();
    }

    // Create and set up the adapter under test
    udp_adapter<float> adapter(SocketAddress(Ipv4::localhost(), 0));
    adapter.register_listener(mock_listener);
    uint16_t bound_port = 0;
    ASSERT_NO_THROW(bound_port = adapter.get_bound_port().GetValue());
    ip::udp::endpoint adapter_endpoint(ip::address_v4::loopback(), bound_port);

    // Create a short test packet and send it
    auto input_data = data::test::generate_random_serialized_data_packet<float>();
    auto send_buffer = buffer(input_data.bytes().begin(), input_data.bytes().size());
    auto bytes_sent = send_socket.send_to(send_buffer, adapter_endpoint);
    ASSERT_EQ(bytes_sent, input_data.bytes().size());

    try {
        auto result = mock_listener->get().GetValue();
        ASSERT_EQ(result.bytes().size(), input_data.bytes().size());
        for (size_t index = 0; index < result.bytes().size(); index++) {
            ASSERT_EQ(result.bytes()[index], input_data.bytes()[index]);
        }
    } catch (const std::exception &ex) {
        FAIL() << ex.what();
    }

    // Repeat a second time
    auto input_data2 = data::test::generate_random_serialized_data_packet<float>();
    auto send_buffer2 = buffer(input_data2.bytes().begin(), input_data2.bytes().size());
    bytes_sent = send_socket.send_to(send_buffer2, adapter_endpoint);
    ASSERT_EQ(bytes_sent, input_data2.bytes().size());

    try {
        auto result2 = mock_listener->get().GetValue();

        ASSERT_EQ(result2.bytes().size(), input_data2.bytes().size());
        for (size_t index = 0; index < result2.bytes().size(); index++) {
            ASSERT_EQ(result2.bytes()[index], input_data2.bytes()[index]);
        }
    } catch (const std::exception &ex) {
        FAIL() << ex.what();
    }
}