//
// Created by Jackson Michael on 26/8/20.
//

#ifndef SEM_MOCK_NETWORK_ADAPTER_HPP
#define SEM_MOCK_NETWORK_ADAPTER_HPP

#include "gmock/gmock.h"

#include "network/adapter.hpp"

namespace sem::fft_accel::test::runtime{

template<typename T>
class mock_network_adapter : public network::adapter {
public:
    //using receive_callback_t = typename network::adapter::receive_callback_t;

    mock_network_adapter() = default;
    ~mock_network_adapter() override = default;

    MOCK_METHOD(Result<void>, send, (data_packet));
    MOCK_METHOD(Result<void>, register_listener, (std::weak_ptr<network::response_packet_listener>));
    MOCK_METHOD(Result<uint16_t>, get_bound_port, (), (const, override));

    MOCK_METHOD(bool, is_active, ());
};

}


#endif //SEM_MOCK_NETWORK_ADAPTER_HPP
