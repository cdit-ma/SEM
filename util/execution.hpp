#ifndef RE_COMMON_UTIL_EXECTION
#define RE_COMMON_UTIL_EXECTION

#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <iostream>

class Execution{
    private:
        std::mutex mutex_;
        bool terminated = false;
        std::condition_variable lock_condition_;

        std::vector<std::function<void()> > terminate_functions_;
        std::vector<std::string> errors_;

    public:
        int Start(){
            //Wait for the signal_handler to notify for exit
            {
                std::unique_lock<std::mutex> lock(mutex_);
                lock_condition_.wait(lock, [this]{return terminated;});
            }
            for(auto func : terminate_functions_){
                func();
            }

            if(errors_.empty()) {
                return 0;
            } else {
                return 1;
            }
        };

        auto GetErrors() -> std::vector<std::string> {
            return errors_;
        }

        void SetError(const std::string& error_string) {
            std::lock_guard<std::mutex> lock(mutex_);
            errors_.emplace_back(error_string);
        }

        void AddTerminateCallback(std::function<void()> callback_func){
            std::unique_lock<std::mutex> lock(mutex_);
            terminate_functions_.push_back(callback_func);
        }

        void Interrupt(){
            std::unique_lock<std::mutex> lock(mutex_);
            terminated = true;
            lock_condition_.notify_all();
        }

        bool IsInterrupted(){
            std::unique_lock<std::mutex> lock(mutex_);
            return terminated;
        }
        
};

#endif //RE_COMMON_UTIL_EXECTION