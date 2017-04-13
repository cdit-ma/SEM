#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>

#include "executionmanager.h"
#include "deploymentmanager.h"

class Execution{
    private:
        std::mutex mutex_;
        std::condition_variable lock_condition_;

        std::vector<std::function<void()> > terminate_functions_;

    public:
        void Start(){
            //Wait for the signal_handler to notify for exit
	        std::unique_lock<std::mutex> lock(mutex_);
	        lock_condition_.wait(lock);
            for(auto func : terminate_functions_){
                func();
            }
        };

        void AddTerminateCallback(std::function<void()> callback_func){
            std::unique_lock<std::mutex> lock(mutex_);
            terminate_functions_.push_back(callback_func);
        }

        void Interrupt(){
            std::unique_lock<std::mutex> lock(mutex_);
            lock_condition_.notify_all();
        };
};