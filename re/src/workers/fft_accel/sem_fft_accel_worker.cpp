//
// Created by Jackson Michael on 19/8/20.
//

#include "sem_fft_accel_worker.hpp"
#include "udp/udp_adapter.hpp"

using namespace sem::fft_accel;

worker::worker(const BehaviourContainer &container, const std::string &inst_name) :
        Worker(container, GET_FUNC, inst_name) {
    auto endpoint_attribute = ConstructAttribute(
            ATTRIBUTE_TYPE::STRING,
            std::string(AttrNames::accelerator_endpoint)
            ).lock();
    if (endpoint_attribute == nullptr) {
        throw std::runtime_error("Unable to create accelerator_endpoint attribute in FFTAccelWorker");
    }
}

void worker::HandleConfigure() {
    auto endpoint = GetAttribute(std::string(AttrNames::accelerator_endpoint)).lock();
    if (endpoint == nullptr) {
        throw std::runtime_error("No accelerator endpoint attribute exists when ");
    }

    adapter_ = std::make_unique<network::udp_adapter<float>>();
    adapter_->register_callback(
            [this](Result <std::vector<float>> result_data) {

            }
    );
    BehaviourContainer::HandleConfigure();
}

const std::string &worker::get_version() const {
    const static std::string worker_version{"0.1.0"};
    return worker_version;
}
