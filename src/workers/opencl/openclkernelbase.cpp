
#include "openclmanager.h"
#include "openclkernelbase.h"

#include <iostream>

OpenCLKernelBase::OpenCLKernelBase(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker) :
        manager_(manager), kernel_(kernel), worker_ref_(worker) {
    name_ = kernel_.getInfo<CL_KERNEL_FUNCTION_NAME>();
}

bool OpenCLKernelBase::Run(unsigned int gpu_num, bool block, const cl::NDRange& offset, const cl::NDRange& global,
    const cl::NDRange& local) {
        
    cl::CommandQueue queue = manager_.GetQueues()[gpu_num];

    cl_int err;
    cl::Event kernel_event;

    err = queue.enqueueNDRangeKernel(kernel_, offset, global, local, NULL, &kernel_event);
    if (err != CL_SUCCESS) {
        LogError(__func__,
            "Failed to enqueue kernel '"+name_+"' for execution", err);
            return false;
    }

    if (block) {
        kernel_event.wait();
    }

    return true;
}

std::string OpenCLKernelBase::GetName() const {
    return name_;
}

cl::Kernel OpenCLKernelBase::GetBackingRef() {
    return kernel_;
}

void OpenCLKernelBase::LogError(std::string function_name, std::string error_message, cl_int cl_error_code) {
    LogOpenCLError(worker_ref_,
        "OpenCLKernelBase::" + function_name,
        error_message,
        cl_error_code);
}

void OpenCLKernelBase::LogError(std::string function_name, std::string error_message) {
    LogOpenCLError(worker_ref_,
        "OpenCLKernelBase::" + function_name,
        error_message);
}