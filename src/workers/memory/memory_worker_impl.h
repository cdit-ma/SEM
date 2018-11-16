#ifndef WORKERS_MEMORY_MEMORYWORKER_IMPL
#define WORKERS_MEMORY_MEMORYWORKER_IMPL

#include <mutex>
#include <list>

class Memory_Worker_Impl{
    public:
        ~Memory_Worker_Impl();
        void Allocate(const size_t kilobytes);
        void Deallocate(const size_t kilobytes);
        size_t GetAllocatedCount();
    private:
        // Locking mechanism for protecting <memory_>.
        std::mutex lock_;
        
        // Collection of memory allocations.
        std::list <char *> memory_;
};

#endif //WORKERS_MEMORY_MEMORYWORKER_IMPL
