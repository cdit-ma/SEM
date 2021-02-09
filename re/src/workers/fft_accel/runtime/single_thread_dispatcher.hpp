//
// Created by Jackson Michael on 10/12/20.
//

#ifndef SEM_FFT_ACCEL_SINGLE_THREAD_DISPATCHER_HPP
#define SEM_FFT_ACCEL_SINGLE_THREAD_DISPATCHER_HPP

#include "event_dispatcher.hpp"

#include <thread>
#include <atomic>
#include <deque>
#include <string>
#include <unordered_map>

#include <iostream>

namespace sem::fft_accel::runtime {

    template<typename EventID, typename EventData>
    class single_thread_dispatcher final : public event_dispatcher<EventID, EventData> {
        using EventDispatcherInterface = event_dispatcher<EventID, EventData>;
        using Callback = typename EventDispatcherInterface::Callback;

    public:
        single_thread_dispatcher();

        ~single_thread_dispatcher() final;

        void stop();

        Result<void> submit_event(EventID id, EventData data) final;

        Result<void> register_callback(Callback callback) final;

        Result<std::future<EventData>> request_event(EventID id) final;

    private:
        void run_dispatch_loop();

        Callback registered_callback_;

        std::deque<std::pair<EventID, EventData>> event_queue_;
        std::mutex queue_mutex_;

        std::unordered_map<EventID, std::promise<EventData>> promise_map_;

        std::atomic<bool> should_continue_running_;
        std::thread dispatch_thread_;
        std::condition_variable event_occurred_;
    };

    template<typename EventID, typename EventData>
    single_thread_dispatcher<EventID, EventData>::single_thread_dispatcher() :
            should_continue_running_(true),
            dispatch_thread_([this]() { run_dispatch_loop(); }) {}

    template<typename EventID, typename EventData>
    single_thread_dispatcher<EventID, EventData>::~single_thread_dispatcher() {
        stop();
        dispatch_thread_.join();
    }

    template<typename EventID, typename EventData>
    void single_thread_dispatcher<EventID, EventData>::stop() {
        std::lock_guard queue_lock(queue_mutex_);
        should_continue_running_ = false;
        // Ensure that the queue variable gets the signal to stop waiting for new events
        event_occurred_.notify_all();
    }

    template<typename EventID, typename EventData>
    Result<void> single_thread_dispatcher<EventID, EventData>::submit_event(EventID id, EventData data) {
        std::lock_guard queue_lock(queue_mutex_);

        auto promise_iter = promise_map_.find(id);
        if (promise_iter != promise_map_.end()) {
            promise_iter->second.set_value(std::move(data));
            promise_map_.erase(promise_iter);
            return {};
        }

        event_queue_.emplace_back(id, std::move(data));
        event_occurred_.notify_one();

        return {};
    }

    template<typename EventID, typename EventData>
    Result<void> single_thread_dispatcher<EventID, EventData>::register_callback(Callback callback) {
        std::lock_guard queue_lock(queue_mutex_);
        try {
            if (!callback) {
                return ErrorResult("An error occurred registering a callback: callback must be valid");
            }
            registered_callback_ = std::move(callback);
        } catch (const std::exception &ex) {
            return ErrorResult(
                    "An exception occurred when single_threaded_dispatcher attempted to register callback:\n" +
                    std::string(ex.what()));
        }
        return {};
    }

    template<typename EventID, typename EventData>
    Result<std::future<EventData>> single_thread_dispatcher<EventID, EventData>::request_event(EventID id) {
        std::lock_guard queue_lock(queue_mutex_);
        try {
            // Construct a new promise for the provided ID
            auto[iter, success] = promise_map_.try_emplace(id);
            if (!success) {
                return ErrorResult("single_thread_dispatcher failed to store promise for request event in map.");
            }
            return iter->second.get_future();
        } catch (const std::exception &ex) {
            return ErrorResult(
                    "An exception occurred when single_threaded_dispatcher attempted to provide a future for a requested event:\n" +
                    std::string(ex.what()));
        }
    }

    template<typename EventID, typename EventData>
    void single_thread_dispatcher<EventID, EventData>::run_dispatch_loop() {

        while (should_continue_running_.load()) {
            std::unique_lock queue_lock(queue_mutex_);

            if (!event_queue_.empty()) {
                if (registered_callback_) {
                    auto event = event_queue_.front();
                    registered_callback_(event.first, event.second);
                    event_queue_.pop_front();
                } else {
                    throw std::runtime_error("Attempting to process an event without having registered a callback");
                }
            } else {
                // If the queue is empty we should wait for the signal that something has happened rather than spinning
                event_occurred_.wait(queue_lock, [this] {
                    return !event_queue_.empty() || !should_continue_running_;
                });
            }
        }
    }

}

#endif //SEM_FFT_ACCEL_SINGLE_THREAD_DISPATCHER_HPP
