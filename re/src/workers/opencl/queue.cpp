#include "queue.h"
#include "manager.h"

#include "utilities.h"

using namespace Re::OpenCL;

// cl_int err_;

Queue::Queue(const Manager& manager, const Device& device) : manager_(manager), dev_(device)
{
    cl_int err;
    queue_ = std::make_unique<cl::CommandQueue>(manager_.GetContext(), dev_.GetRef(), CL_QUEUE_PROFILING_ENABLE, &err);
    if(err != CL_SUCCESS) {
        throw OpenCLException("Failed to create queue", err);
    }
}

cl::CommandQueue& Queue::GetRef() const { return *queue_; }