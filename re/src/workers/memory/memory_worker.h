#ifndef WORKERS_MEMORY_MEMORYWORKER
#define WORKERS_MEMORY_MEMORYWORKER

#include <memory>
#include "worker.h"

class Memory_Worker_Impl;
class Memory_Worker : public Worker{
    public:
        Memory_Worker(const BehaviourContainer& container, const std::string& inst_name);
        ~Memory_Worker();
        const std::string& get_version() const override;
        
        void Allocate(int kilobytes);
        void Deallocate(int kilobytes);
        int GetAllocatedCount() const;
    private:
        std::unique_ptr<Memory_Worker_Impl> impl_;
};

#endif  //WORKERS_MEMORY_MEMORYWORKER
