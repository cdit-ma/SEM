//
// Created by Jackson Michael on 31/8/20.
//

#include "worker_adapter.hpp"

using namespace sem;
using namespace sem::fft_accel::runtime;

worker_adapter::worker_adapter(const BehaviourContainer &container, const std::string &inst_name)
        : Worker(container, GET_FUNC, inst_name) {

    auto endpoint_attribute = ConstructAttribute(
            ATTRIBUTE_TYPE::STRING,
            std::string(AttrNames::accelerator_endpoint)
    ).lock();

    if (endpoint_attribute == nullptr) {
        throw std::runtime_error("Unable to create accelerator_endpoint attribute in FFTAccelWorker");
    }
}

const std::string &worker_adapter::get_version() const {
    const static std::string worker_version{"0.1.0"};
    return worker_version;
}

Result<void> worker_adapter::set_network_adapter(std::weak_ptr<network::adapter> network_adapter) {
    try {
        auto&& locked_ptr = network_adapter.lock();
        if (locked_ptr) {
            adapter_ = locked_ptr;
            adapter_->register_listener(weak_from_this());
            return {};
        } else {
            return ErrorResult{"set_network_adapter: Unable to lock the provided network adapter pointer"};
        }
    } catch (const std::exception &ex) {
        return ErrorResult(
                std::string("set_network_adapter threw a std::exception while assigning adapter shared pointer: ") +
                ex.what());
    } catch (...) {
        return ErrorResult(
                std::string("set_network_adapter threw an unknown exception while assigning adapter shared pointer"));
    }
}

Result<void> worker_adapter::receive_processed_fft(std::vector<float> data) {
    return ErrorResult("receive_processed_fft not yet implemented");
}
