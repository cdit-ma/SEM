//
// Created by Jackson Michael on 19/8/20.
//

#include "sem_fft_accel_worker.hpp"
#include "network/udp/udp_adapter.hpp"

using namespace sem::fft_accel;

sem::fft_accel::Worker::Worker(const BehaviourContainer &container, const std::string &inst_name) :
        ::Worker(container, GET_FUNC, inst_name),
        runtime_adapter_(std::make_shared<runtime::adapter_impl>())
        {
    auto endpoint_attribute = ConstructAttribute(
            ATTRIBUTE_TYPE::STRING,
            std::string(AttrNames::accelerator_endpoint)
            ).lock();
    if (endpoint_attribute == nullptr) {
        throw std::runtime_error("Unable to create accelerator_endpoint attribute in FFTAccelWorker");
    }
}


void sem::fft_accel::Worker::HandleConfigure() {
    auto endpoint = GetAttribute(std::string(AttrNames::accelerator_endpoint)).lock();
    if (endpoint == nullptr) {
        throw std::runtime_error("No accelerator endpoint attribute exists when ");
    }

    auto&& fae_addr = re::types::SocketAddress(endpoint->String());
    network_adapter_ = std::make_shared<network::udp_adapter<float>>(fae_addr);

    runtime_adapter_->set_network_adapter(network_adapter_);

    // Ensure that base behaviour is called
    BehaviourContainer::HandleConfigure();
}

const std::string& sem::fft_accel::Worker::get_version() const {
    const static std::string worker_version{"0.1.0"};
    return worker_version;
}
