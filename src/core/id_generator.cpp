#include "id_generator.h"
#include <mutex>

int IdGenerator::get_next_id(){
    static std::mutex mutex_;
    static int counter_ = 0;
    std::lock_guard<std::mutex> lock(mutex_);
    return ++counter_;
}