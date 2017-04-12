#include <mutex>
#include <condition_variable>

#include "executionmanager.h"
#include "deploymentmanager.h"

class Execution{
    private:
        std::mutex mutex_;
        std::condition_variable lock_condition_;

        ExecutionManager* em_;
        DeploymentManager* dm_;

    public:
        void Start(DeploymentManager* dm, ExecutionManager* em){
            em_ = em;
            dm_ = dm;
            //Wait for the signal_handler to notify for exit
	        std::unique_lock<std::mutex> lock(mutex_);
	        lock_condition_.wait(lock);
        };

        void Interrupt(){
            std::unique_lock<std::mutex> lock(mutex_);
            //Check that both execution manager and deployment manager have finished (or weren't ever running on this node)
            if((em_ == 0 || em_->Finished()) && (dm_ == 0 || dm_->get_deployment() == 0)){
	            lock_condition_.notify_all();
            }
        };

        void HardInterrupt(){
            std::unique_lock<std::mutex> lock(mutex_);
            lock_condition_.notify_all();
        };
};