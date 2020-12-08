//
// Created by Jackson Michael on 19/8/20.
//

#ifndef SEM_SEM_FFT_ACCEL_WORKER_H
#define SEM_SEM_FFT_ACCEL_WORKER_H

#include "worker.h"
#include "runtime/adapter_impl.hpp"
#include "network/udp/udp_adapter.hpp"

namespace sem::fft_accel {
class Worker : public ::Worker {
public:
    Worker(const BehaviourContainer &container, const std::string &inst_name);

    const std::string &get_version() const override;

    std::vector<float> calculate_fft(const std::vector<float>& data);
    uint16_t calculate_fft_async(const std::vector<float>& data);


    void SetResponseCallback(std::function<void (uint8_t, const std::vector<float> &)> func);

    /**
     * A grouping of constant component attribute names
     */
    struct AttrNames {
        /// The qualified endpoint (eg ip address + port) for the FFT Acceleration Engine
        constexpr static std::string_view accelerator_endpoint{"accelerator_endpoint"};
    };

protected:
    void HandleConfigure() override;


private:
    std::shared_ptr<runtime::adapter> runtime_adapter_;
    std::shared_ptr<network::adapter> network_adapter_;

    std::function<void (uint8_t, std::vector<float>)> callback_function_;
};
}

// Expose the worker class to the global namespace as codegen doesn't currently support namespacing
using FFTAccelWorker = sem::fft_accel::Worker;

#endif //SEM_SEM_FFT_ACCEL_WORKER_H
