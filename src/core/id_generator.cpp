#include "id_generator.h"
#include <atomic>

namespace IdGenerator{
    uint64_t get_next_id(){
        static std::atomic<uint64_t> counter_{0};
        return ++counter_;
    };
};