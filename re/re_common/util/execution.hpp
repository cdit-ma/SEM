#ifndef RE_COMMON_UTIL_EXECUTION
#define RE_COMMON_UTIL_EXECUTION

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

class Execution {
private:
    std::mutex mutex_;
    bool terminated = false;
    std::condition_variable lock_condition_;

    std::vector<std::function<void()>> terminate_functions_;

public:
    void Start()
    {
        // Wait for the signal_handler to notify for exit
        {
            std::unique_lock<std::mutex> lock(mutex_);
            lock_condition_.wait(lock, [this] { return terminated; });
        }
        std::ostringstream error_messages;
        for(const auto& func : terminate_functions_) {
            try {
                // Catch, log and move on if we encounter any exceptions.
                // This is so that we are sure we've called all termination callbacks regardless of
                // others failing.
                func();
            } catch(const std::exception& ex) {
                error_messages << ex.what() << std::endl;
                std::cerr << ex.what() << std::endl;
            }
        }
        if(!error_messages.str().empty()) {
            throw std::logic_error(error_messages.str());
        }
    }

    void AddTerminateCallback(const std::function<void()>& callback_func)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        terminate_functions_.emplace_back(callback_func);
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
