//
// Created by Jackson Michael on 27/10/20.
//

#ifndef SEM_ASYNC_FFT_RESULT_LISTENER_HPP
#define SEM_ASYNC_FFT_RESULT_LISTENER_HPP

#include "network/adapter.hpp"
#include "result.hpp"

#include <future>

#include <queue>

namespace sem::fft_accel::network::test {

    class async_fft_result_listener : public response_packet_listener {
    public:
        explicit async_fft_result_listener(std::chrono::milliseconds timeout) : timeout_(timeout){}

        sem::Result<void> receive_response_packet(data_packet data) override {
            std::unique_lock lockGuard(cv_fft_processed_mutex_);
            packet_queue_.emplace(std::move(data));
            fft_processed_.notify_one();
            return {};
        };

        Result <data::serialized_fft_data<float>> get() {
            std::unique_lock lockGuard(cv_fft_processed_mutex_);
            if (packet_queue_.empty()) {
                auto wait_status = fft_processed_.wait_for(lockGuard, timeout_);
                if (wait_status == std::cv_status::timeout) {
                    return ErrorResult("async_fft_result_listener timed out");
                }
            }
            auto vec = std::move(packet_queue_.front());
            packet_queue_.pop();
            return {vec};
        }

    private:
        std::mutex cv_fft_processed_mutex_;
        std::condition_variable fft_processed_;
        std::queue<data::serialized_fft_data<float>> packet_queue_;
        std::chrono::milliseconds timeout_;
    };

}

#endif //SEM_ASYNC_FFT_RESULT_LISTENER_HPP
