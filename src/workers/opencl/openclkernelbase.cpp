
#include "openclmanager.h"
#include "openclkernelbase.h"

#include "openclutilities.h"
#include "openclexception.h"

#include <iostream>

OpenCLKernelBase::OpenCLKernelBase(const Worker& worker, OpenCLManager& manager, cl::Kernel& kernel) :
        manager_(manager), kernel_(std::make_shared<cl::Kernel>(kernel)), worker_ref_(worker) {
    name_ = kernel_->getInfo<CL_KERNEL_FUNCTION_NAME>();
}

void OpenCLKernelBase::Run(const OpenCLDevice& device, bool block, const cl::NDRange& offset, const cl::NDRange& global,
    const cl::NDRange& local) {
    
    auto& queue = device.GetQueue();

    cl_int err;
    cl::Event kernel_event;

    err = queue.enqueueNDRangeKernel(*kernel_, offset, global, local, NULL, &kernel_event);
    if (err != CL_SUCCESS) {
        throw OpenCLException(GET_FUNC ": Failed to enqueue kernel '"+name_+"' for execution", err);
    }

    if (block) {
        kernel_event.wait();
    }
}

void OpenCLKernelBase::SetArg(unsigned int index, size_t size, const void* value) {
    cl_int err;
    
    // If we are passed something derived from GenericBuffer we know the backing reference will be properly handled
    err = kernel_->setArg(index, size, value);
    if (err != CL_SUCCESS) {
        throw OpenCLException(GET_FUNC "Unable to set parameter "+std::to_string(index)+" of a kernel", err);
    }
}

void OpenCLKernelBase::SetArg(unsigned int index, const cl::Memory& mem_obj) {
    cl_int err;
    
    err = kernel_->setArg(index, mem_obj);
    if (err != CL_SUCCESS) {
        throw OpenCLException(GET_FUNC "Unable to set parameter "+std::to_string(index)+" of a kernel", err);
    }
}

void OpenCLKernelBase::SetArg(unsigned int index, const cl::Buffer& buf_obj) {
    SetArg(index, (const cl::Memory&) buf_obj);
}

void OpenCLKernelBase::SetArg(unsigned int index, const cl::LocalSpaceArg& local_space) {
    cl_int err;
    
    err = kernel_->setArg(index, local_space);
    if (err != CL_SUCCESS) {
        throw OpenCLException(GET_FUNC "Unable to set parameter "+std::to_string(index)+" of a kernel", err);
    }
}

std::string OpenCLKernelBase::GetName() const {
    return name_;
}

cl::Kernel OpenCLKernelBase::GetBackingRef() {
    return *kernel_;
}

void OpenCLKernelBase::LogError(const Worker& worker, std::string function_name, std::string error_message, cl_int cl_error_code) {
    LogOpenCLError(worker,
        "OpenCLKernelBase::" + function_name,
        error_message,
        cl_error_code);
}

void OpenCLKernelBase::LogError(const Worker& worker, std::string function_name, std::string error_message) {
    LogOpenCLError(worker,
        "OpenCLKernelBase::" + function_name,
        error_message);
}