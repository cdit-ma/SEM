#ifndef RE_COMMON_UTIL_EXECUTION
#define RE_COMMON_UTIL_EXECUTION

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <vector>

class Execution {
private:
    std::mutex mutex_;
    bool terminated = false;
    std::condition_variable lock_condition_;

    std::vector<std::function<void()>> terminate_functions_;
    std::vector<std::unique_ptr<std::exception>> errors_;

public:
    void Start()
    {
        // Wait for the signal_handler to notify for exit
        {
            std::unique_lock<std::mutex> lock(mutex_);
            lock_condition_.wait(lock, [this] { return terminated; });
        }
        for(auto func : terminate_functions_) {
            func();
        }

        // Re-acquire mutex while we populate our error message (if we have one)
        std::lock_guard<std::mutex> lock(mutex_);
        if(!errors_.empty()) {
            auto message = std::string(errors_.front()->what());
            for(auto it = std::next(errors_.begin()); it != errors_.end(); ++it) {
                message += '\n';
                message += (*it)->what();
            }

            throw std::runtime_error(message);
        }
    }

    void AddException(std::unique_ptr<std::exception> error)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.emplace_back(std::move(error));
    }

    void AddTerminateCallback(std::function<void()> callback_func)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        terminate_functions_.push_back(callback_func);
    }

    void Interrupt()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        terminated = true;
        lock_condition_.notify_all();
    }

    bool IsInterrupted()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return terminated;
    }
};

#endif // RE_COMMON_UTIL_EXECUTION
