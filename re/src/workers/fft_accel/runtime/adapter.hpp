//
// Created by Jackson Michael on 26/8/20.
//

#ifndef SEM_RUNTIME_ADAPTER_HPP
#define SEM_RUNTIME_ADAPTER_HPP

#include "network/adapter.hpp"

#include "result.hpp"

namespace sem::fft_accel::runtime {

class adapter : public network::fft_result_listener {

public:
    virtual ~adapter() = 0;

    //template <typename SampleType>
    virtual Result<void> set_network_adapter(std::weak_ptr<network::adapter> network_adapter) = 0;

};

}

#endif //SEM_RUNTIME_ADAPTER_HPP
