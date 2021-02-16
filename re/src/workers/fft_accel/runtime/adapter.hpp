//
// Created by Jackson Michael on 26/8/20.
//

#ifndef SEM_RUNTIME_ADAPTER_HPP
#define SEM_RUNTIME_ADAPTER_HPP

#include "network/adapter.hpp"

#include "result.hpp"
#include "timeout.hpp"

#include <functional>

namespace sem::fft_accel::runtime {

/**
 * The runtime adapter is responsible for breaking data requests into appropriately structured packets,
 * tracking the packets that have been sent and received, as well as returning the result of an FFT
 * execution request once all the associated response packets have been received.
 */
class adapter : public network::response_packet_listener {

public:
    using CallbackSignature = void (data::data_request_id , std::vector<float>);
    using Callback = std::function<CallbackSignature>;

    virtual ~adapter() = 0;

    /**
     * Causes the runtime adapter to register with a network adapter in order to transmit packets
     * across the network
     * @param network_adapter
     * @return void on success,
     *         ErrorResult on failure to properly set and register with the network adapter
     */
    virtual Result<void> set_network_adapter(std::weak_ptr<network::adapter> network_adapter) = 0;

    /**
     * Submit the data over which an FFT calculation will be performed. This involves splitting the data up into packets
     * which are then sent via a network adapter
     * @param data
     * @return The ID of the newly created request on success,
     *         an ErrorResult if the FFT request is not able to be successfully submitted
     */
    virtual Result<data::data_request_id> submit_fft_calculation(const std::vector<float>& data) = 0;
    virtual Result<data::data_request_id> submit_fft_calculation_async(const std::vector<float>& data) = 0;

    virtual Result<void> register_result_callback(Callback callback) = 0;

    virtual Result<std::vector<float>> wait_on_request_completion(data::data_request_id id, sem::types::Timeout timeout) = 0;


};

}

#endif //SEM_RUNTIME_ADAPTER_HPP
