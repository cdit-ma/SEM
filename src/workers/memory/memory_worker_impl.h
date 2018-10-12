#ifndef WORKERS_MEMORY_MEMORYWORKER_IMPL
#define WORKERS_MEMORY_MEMORYWORKER_IMPL

#include <mutex>
#include <list>

class Memory_Worker_Impl{
    public:
        ~Memory_Worker_Impl();
        bool Allocate(size_t kilobytes);
        bool Deallocate(size_t kilobytes);
        size_t GetAllocatedCount();

    private:
        bool _NonThreadSafeDeallocate(size_t kilobytes);

         /// Type definition for the container of allocations.
        typedef std::list <char *> Memory_Allocations;

        /// Locking mechanism for protecting <memory_>.
        std::mutex lock_;

        /// Collection of memory allocations.
        Memory_Allocations memory_;
};

#endif //WORKERS_MEMORY_MEMORYWORKER_IMPL
