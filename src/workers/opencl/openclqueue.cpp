#include "openclqueue.h"
#include "openclmanager.h"

#include "openclutilities.h"

cl_int err_;

OpenCLQueue::OpenCLQueue(const OpenCLManager& manager, const OpenCLDevice& device) :
    manager_(manager),
    dev_(device)
    //queue_(new cl::CommandQueue(manager_.GetContext(), dev_.GetRef(), CL_QUEUE_PROFILING_ENABLE, &err_)){
    {
        
        queue_ = std::make_shared<cl::CommandQueue>(manager_.GetContext(), dev_.GetRef(), CL_QUEUE_PROFILING_ENABLE, &err_);
    
}

cl::CommandQueue& OpenCLQueue::GetRef() const {
    return *queue_;
}