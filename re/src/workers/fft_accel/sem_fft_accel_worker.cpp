//
// Created by Jackson Michael on 19/8/20.
//

#include "sem_fft_accel_worker.hpp"
#include "network/udp/udp_adapter.hpp"

using namespace sem::fft_accel;

using namespace std::string_literals;

namespace sem::fft_accel::detail {
    bool is_power_of_two(size_t value) {
        return (value & (value - 1)) == 0;
    }
}

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

std::vector<float> sem::fft_accel::Worker::calculate_fft(const std::vector<float> &data) {
    auto work_id = get_new_work_id();
    Log(GET_FUNC , Logger::WorkloadEvent::STARTED, work_id,
        "Called calculate_fft with vector of length "+std::to_string(data.size()));

    // If we have an empty vector as input log an error and return an empty vector
    if (data.empty()) {
        Log(GET_FUNC , Logger::WorkloadEvent::Error, work_id,
            "Cannot calculate FFT for vector of length 0");
        return {};
    }

    if (!detail::is_power_of_two(data.size())) {
        Log(GET_FUNC , Logger::WorkloadEvent::Error, work_id,
            "Cannot calculate FFT for vector that is not a power of 2");
        return {};
    }

    auto submission_result = runtime_adapter_->submit_fft_calculation(data);
    if (submission_result.is_error()) {
        Log(GET_FUNC , Logger::WorkloadEvent::Error, work_id,
            "An error occurred while submitting FFT for calculation:\n"s + submission_result.GetError().msg);
        return {};
    }
    auto request_id = submission_result.GetValue();

    auto completion_result = runtime_adapter_->wait_on_request_completion(request_id, std::chrono::milliseconds(500));
    if (completion_result.is_error()) {
        Log(GET_FUNC , Logger::WorkloadEvent::Error, work_id,
            "An error occurred while waiting for result of FFT for calculation:\n"s + completion_result.GetError().msg);
        return {};
    }

    Log(GET_FUNC , Logger::WorkloadEvent::FINISHED, work_id);
    return {completion_result.GetValue()};
}

uint16_t sem::fft_accel::Worker::calculate_fft_async(const std::vector<float> &data) {
    auto work_id = get_new_work_id();
    Log(GET_FUNC , Logger::WorkloadEvent::STARTED, work_id,
        "Called calculate_fft_async with vector of length "+std::to_string(data.size()));

    // If we have an empty vector as input log an error and return an empty vector
    if (data.empty()) {
        Log(GET_FUNC , Logger::WorkloadEvent::Error, work_id,
            "Cannot calculate FFT for vector of length 0");
        return {};
    }

    if (!detail::is_power_of_two(data.size())) {
        Log(GET_FUNC , Logger::WorkloadEvent::Error, work_id,
            "Cannot calculate FFT for vector that is not a power of 2");
        return {};
    }

    auto result = runtime_adapter_->submit_fft_calculation(data);
    if (result.is_error()) {
        Log(GET_FUNC , Logger::WorkloadEvent::Error, work_id,
            "An error occurred while submitting FFT for calculation: "s + result.GetError().msg);
        return {};
    }

    // TODO: push request future onto a map and hook into runtime adapter for signal to fire callback

    Log(GET_FUNC , Logger::WorkloadEvent::FINISHED, work_id);
    return result.GetValue();
}

void sem::fft_accel::Worker::SetResponseCallback(std::function<void(uint8_t, const std::vector<float> &)> func) {
    callback_function_ = func;
}
