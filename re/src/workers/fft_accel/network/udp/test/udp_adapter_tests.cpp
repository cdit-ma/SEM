//
// Created by Jackson Michael on 3/9/20.
//

#include "gtest/gtest.h"

#include "network/udp/udp_adapter.hpp"
#include "async_fft_result_listener.hpp"

#include <thread>

#include <iostream>

using namespace re::types;
using namespace sem::fft_accel::network;
using namespace boost::asio;

using namespace testing;


TEST(fft_accel_udp_adapter, nothrow_constructor) {
    auto test_addr = re::types::SocketAddress("123.234.123.234:12345");
    ASSERT_NO_THROW(udp_adapter<float> test_adapter(test_addr););
}

TEST(fft_accel_udp_adapter, send_short_vec_over_udp) {
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
                       [&test_io_service, &listen_socket]() -> sem::Result<std::vector<float>> {
                           std::vector<float> listen_vec(8);
                           listen_socket.receive(boost::asio::buffer(listen_vec));

                           test_io_service.run();
                           return {listen_vec};
                       });

    udp_adapter<float> test_adapter(SocketAddress(Ipv4::localhost(), bind_port));
    std::vector<float> input_data{0, 1, 2, 3, 4, 5, 6, 7};

    auto send_result = test_adapter.send(input_data);
    send_result.GetValue();

    auto result = listen_future.get();
    ASSERT_FALSE(result.is_error());

    auto &&output_data = result.GetValue();
    ASSERT_EQ(output_data.size(), input_data.size());
    ASSERT_EQ(output_data, input_data);

    auto send_result2 = test_adapter.send(input_data);
    send_result2.GetValue();
}

TEST(fft_accel_udp_adapter, extern_comms_test) {
    udp_adapter<float> extern_adapter(SocketAddress(Ipv4::localhost(), 21212));
    extern_adapter.send(std::vector<float>{1,2,3,4,5,6,7,8});
}

TEST(fft_accel_udp_adapter, receive_short_vec_over_udp) {
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
    uint16_t bound_port;
    ASSERT_NO_THROW(bound_port = adapter.get_bound_port().GetValue());
    ip::udp::endpoint adapter_endpoint(ip::address_v4::loopback(), bound_port);

    // Create a short test vector and send it
    std::vector<float> input_data{1, 2, 3, 4, 5, 6, 7, 8};
    auto bytes_sent = send_socket.send_to(buffer(input_data), adapter_endpoint);

    auto result = mock_listener->get().GetValue();
    ASSERT_EQ(result.size(), input_data.size());
    ASSERT_EQ(result, input_data);

    std::vector<float> input_data2{1, 3, 5, 7, 9, 11, 13, 15};
    bytes_sent = send_socket.send_to(buffer(input_data2), adapter_endpoint);

    try {
        auto result2 = mock_listener->get().GetValue();

        ASSERT_EQ(result2.size(), input_data2.size());
        ASSERT_EQ(result2, input_data2);
    } catch (const std::exception& ex) {
        FAIL() << ex.what();
    }
}