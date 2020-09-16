//
// Created by Jackson Michael on 3/9/20.
//

#include "gtest/gtest.h"

#include "network/udp/udp_adapter.hpp"

using namespace sem::fft_accel::network;

TEST(fft_accel_udp_adapter, nothrow_constructor) {
    auto test_addr = re::types::SocketAddress::from_string("123.234.123.234:12345");
    ASSERT_NO_THROW(udp_adapter<float> test_adapter(test_addr););
}

