//
// Created by Jackson Michael on 31/8/20.
//

#include "adapter_impl.hpp"
#include "data/fft_data.hpp"
#include <string>

using namespace sem;
using namespace sem::types;
using namespace sem::fft_accel::runtime;
using namespace sem::fft_accel::data;

using namespace std::literals::string_literals;

adapter_impl::adapter_impl() = default;

Result<void> adapter_impl::set_network_adapter(std::weak_ptr<network::adapter> network_adapter) {
    try {
        auto &&locked_ptr = network_adapter.lock();
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

Result<void> adapter_impl::receive_response_packet(data_packet data) {
    try {
        data::fft_data_packet<float> deserialized_data(data);
        auto update_result = pending_requests_.update_pending_packets(deserialized_data);
        if (update_result.is_error()) {
            return ErrorResult("Runtime adapter failed to update pending packets:\n"s + update_result.GetError().msg);
        }

        auto update_data = update_result.GetValue();
        if (update_data.has_value()) {
            auto completed_request = *update_data;
            event_dispatcher.submit_event(completed_request.id, completed_request.data);
        }
        return {};
    } catch (const std::exception &ex) {
        return ErrorResult(
                "Exception in FFT Runtime Adapter when attempting to handle a received packet:\n"s + ex.what());
    }
}

Result<data_request_id> adapter_impl::submit_fft_calculation(const std::vector<float> &data) {
    if (!adapter_) {
        return ErrorResult("Runtime Adapter unable to calculate FFT: no network adapter registered.");
    }

    auto &&new_request_result = pending_requests_.construct_packets(data);
    if (new_request_result.is_error()) {
        return ErrorResult("Runtime Adapter unable to calculate FFT, failed to construct packet group: \n"s +
                           new_request_result.GetError().msg);
    }

    auto packets_group = new_request_result.GetValue().packet_group;
    request_id new_id = packets_group.request_id();

    auto request_future_result = event_dispatcher.request_event(new_id);
    if (request_future_result.is_error()) {
        return ErrorResult("Error occurred retrieving request future in Runtime Adapter: \n"s +
                           new_request_result.GetError().msg);
    }
    response_futures_.try_emplace(new_id, std::move(request_future_result.GetValue()));

    for (const auto &packet : packets_group.packets()) {
        auto &&res = adapter_->send(data_packet{packet});
        if (res.is_error()) {
            return ErrorResult(
                    "Runtime Adapter unable to calculate FFT, error occurred while sending packet group:\n"s +
                    res.GetError().msg);
        }
    }
    return {new_id};
}

Result<data_request_id> adapter_impl::submit_fft_calculation_async(const std::vector<float> &data) {
    if (!adapter_) {
        return ErrorResult("Runtime Adapter unable to calculate FFT: no network adapter registered.");
    }

    auto &&new_request_result = pending_requests_.construct_packets(data);
    if (new_request_result.is_error()) {
        return ErrorResult("Runtime Adapter unable to calculate FFT, failed to construct packet group: \n"s +
                           new_request_result.GetError().msg);
    }

    auto packets_group = new_request_result.GetValue().packet_group;
    request_id new_id = packets_group.request_id();

    for (const auto &packet : packets_group.packets()) {
        auto &&res = adapter_->send(data_packet{packet});
        if (res.is_error()) {
            return ErrorResult(
                    "Runtime Adapter unable to calculate FFT, error occurred while sending packet group:\n"s +
                    res.GetError().msg);
        }
    }
    return {new_id};
}

Result<std::vector<float>>
adapter_impl::wait_on_request_completion(fft_accel::data::data_request_id id, types::Timeout timeout) {
    try {
        // If we don't want to block forever then perform a timeout check
        if (!std::holds_alternative<NeverTimeout>(timeout)) {
            auto millis = std::get<std::chrono::milliseconds>(timeout);
            auto &&timeout_result = response_futures_.at(id).wait_for(millis);
            switch (timeout_result) {
                case std::future_status::timeout:
                    return ErrorResult(
                            "Timed out while waiting for request (" + std::to_string(millis.count()) + "ms)");
                case std::future_status::ready:
                    [[fallthrough]];
                case std::future_status::deferred:
                    break;
            }
        }

        auto response = response_futures_.at(id).get();
//        return {response.to_vector()};
        return {response};
    } catch (const std::exception &ex) {
        return ErrorResult("Failed to get response while waiting for request to complete: "s + ex.what());
    }
}

Result<void> adapter_impl::register_result_callback(Callback callback) {
    auto res = event_dispatcher.register_callback(std::move(callback));
    return res;
}
