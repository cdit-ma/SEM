//
// Created by Jackson Michael on 31/8/20.
//

#include "adapter_impl.hpp"
#include "data/fft_data.hpp"
#include <string>

using namespace sem;
using namespace sem::fft_accel::runtime;

using namespace std::literals::string_literals;

adapter_impl::adapter_impl() {
}

Result<void> adapter_impl::set_network_adapter(std::weak_ptr<network::adapter> network_adapter) {
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
                "set_network_adapter threw a std::exception while assigning adapter shared pointer: "s +
                ex.what());
    } catch (...) {
        return ErrorResult{
                "set_network_adapter threw an unknown exception while assigning adapter shared pointer"};
    }
}

Result<void> adapter_impl::receive_processed_fft(std::vector<float> data) {
    return ErrorResult("receive_processed_fft not yet implemented");
}

Result<void> adapter_impl::submit_fft_calculation(const std::vector<float> &data) {
    if (!adapter_) {
        return ErrorResult("Runtime Adapter unable to calculate FFT: no network adapter registered.");
    }

    // TODO: Add the calculation request in a map so that it can be matched with a response
    auto&& new_packet = pending_requests_.construct_packet(data);

    auto&& res = adapter_->send(new_packet.GetValue().);
    if (res.is_error()) {
        return ErrorResult("Runtime Adapter unable to calculate FFT: \n"s + res.GetError().msg);
    }



    return {};
}
