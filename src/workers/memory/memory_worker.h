#ifndef WORKERS_MEMORY_MEMORYWORKER
#define WORKERS_MEMORY_MEMORYWORKER

#include <core/worker.h>

class Memory_Worker_Impl;

class Memory_Worker : public Worker{
    public:
        Memory_Worker(const BehaviourContainer& container, const std::string& inst_name);
        ~Memory_Worker();
        
        void Allocate(size_t kilobytes);
        void Deallocate(size_t kilobytes);
        size_t GetAllocatedCount() const;

    private:
        Memory_Worker_Impl* impl_ = 0;
};

#endif  //WORKERS_MEMORY_MEMORYWORKER
