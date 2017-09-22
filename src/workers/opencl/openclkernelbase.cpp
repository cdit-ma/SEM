
#include "openclmanager.h"
#include "openclkernelbase.h"

#include <iostream>

OpenCLKernelBase::OpenCLKernelBase(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker) :
        manager_(manager), kernel_(kernel), worker_ref_(worker) {
    
    name_ = kernel_.getInfo<CL_KERNEL_FUNCTION_NAME>();
}

void OpenCLKernelBase::Run(unsigned int gpu_num, bool block, const cl::NDRange& offset, const cl::NDRange& global,
    const cl::NDRange& local) {
        
    cl::CommandQueue queue = manager_.GetQueues()[gpu_num];
    queue.enqueueNDRangeKernel(kernel_, offset, global, local);
}

std::string OpenCLKernelBase::GetName() const {
    return name_;
}

cl::Kernel OpenCLKernelBase::GetBackingRef() {
    return kernel_;
}