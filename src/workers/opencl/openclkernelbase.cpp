
#include "openclmanager.h"
#include "openclkernelbase.h"

#include "openclutilities.h"

#include <iostream>

OpenCLKernelBase::OpenCLKernelBase(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker) :
        manager_(manager), kernel_(std::make_shared<cl::Kernel>(kernel)), worker_ref_(worker) {
    name_ = kernel_->getInfo<CL_KERNEL_FUNCTION_NAME>();
}

bool OpenCLKernelBase::Run(const OpenCLDevice& device, bool block, const cl::NDRange& offset, const cl::NDRange& global,
    const cl::NDRange& local) {
        
    //auto queue = manager_.GetQueues()[gpu_num];
    auto& queue = device.GetQueue();

    cl_int err;
    cl::Event kernel_event;

    err = queue.enqueueNDRangeKernel(*kernel_, offset, global, local, NULL, &kernel_event);
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

bool OpenCLKernelBase::SetArg(unsigned int index, size_t size, const void* value) {
    cl_int err;
    std::cout << kernel_->getArgInfo<CL_KERNEL_ARG_TYPE_NAME>(index, &err) << std::endl;
    
    // If we are passed something derived from GenericBuffer we know the backing reference will be properly handled
    //err = kernel_->setArg(index, size, value);
    std::cout << size << std::endl;
    err = clSetKernelArg(kernel_->get(), index, size, value);
    //err = clSetKernelArg(kernel_->get(), index, 4, value);
    if (err != CL_SUCCESS) {
        LogError(__func__,
            "Unable to set parameter "+std::to_string(index)+" of a kernel",
            err);
        return false;
    }
    //std::cerr << "set arg " << index << std::endl;
    return true;
}

bool OpenCLKernelBase::SetArg(unsigned int index, const cl::Memory& mem_obj) {
    cl_int err;
    
    // If we are passed something derived from GenericBuffer we know the backing reference will be properly handled
    err = kernel_->setArg(index, mem_obj);
    if (err != CL_SUCCESS) {
        LogError(__func__,
            "Unable to set parameter "+std::to_string(index)+" of a kernel",
            err);
        return false;
    }
    //std::cerr << "set arg " << index << std::endl;
    return true;
}

bool OpenCLKernelBase::SetArg(unsigned int index, const cl::Buffer& buf_obj) {
    return SetArg(index, (const cl::Memory&) buf_obj);
}

bool OpenCLKernelBase::SetArg(unsigned int index, const cl::LocalSpaceArg& local_space) {
    cl_int err;
    
    // If we are passed something derived from GenericBuffer we know the backing reference will be properly handled
    err = kernel_->setArg(index, local_space);
    if (err != CL_SUCCESS) {
        LogError(__func__,
            "Unable to set parameter "+std::to_string(index)+" of a kernel",
            err);
        return false;
    }
    //std::cerr << "set arg " << index << std::endl;
    return true;
}

std::string OpenCLKernelBase::GetName() const {
    return name_;
}

cl::Kernel OpenCLKernelBase::GetBackingRef() {
    return *kernel_;
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