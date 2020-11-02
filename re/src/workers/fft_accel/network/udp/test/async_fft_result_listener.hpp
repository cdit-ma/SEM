//
// Created by cdit-ma on 27/10/20.
//

#ifndef SEM_ASYNC_FFT_RESULT_LISTENER_HPP
#define SEM_ASYNC_FFT_RESULT_LISTENER_HPP

#include "network/adapter.hpp"
#include "result.hpp"

#include <future>

#include <queue>

namespace sem::fft_accel::network {

    class async_fft_result_listener : public fft_result_listener {
    public:
        async_fft_result_listener() {
            /*result_future_ = std::async([this](){
                std::unique_lock lockGuard(cv_fft_processed_mutex_);
                fft_processed_.wait(lockGuard);
                return result_;
            });*/
        }

        sem::Result<void> receive_processed_fft(std::vector<float> data) override {
            std::unique_lock lockGuard(cv_fft_processed_mutex_);
            vec_queue.emplace(std::move(data));
            fft_processed_.notify_one();
            return {};
        };

        Result <std::vector<float>> get() {
            std::unique_lock lockGuard(cv_fft_processed_mutex_);
            if (vec_queue.empty()) {
                auto wait_status = fft_processed_.wait_for(lockGuard, std::chrono::milliseconds(300));
                if (wait_status == std::cv_status::timeout) {
                    return ErrorResult("async_fft_result_listener timed out after waiting for 300ms");
                }
            }
            auto vec = vec_queue.front();
            vec_queue.pop();
            return {vec};
        }

    private:
        std::mutex cv_fft_processed_mutex_;
        std::condition_variable fft_processed_;
        std::vector<float> result_;
        std::future<std::vector<float>> result_future_;
        std::queue<std::vector<float>> vec_queue;
    };

}

#endif //SEM_ASYNC_FFT_RESULT_LISTENER_HPP
