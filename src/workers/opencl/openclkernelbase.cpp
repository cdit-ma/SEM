
#include "openclmanager.h"
#include "openclkernelbase.h"

#include "openclutilities.h"
#include "openclexception.h"

#include <iostream>

#include <mutex>

OpenCLKernelBase::OpenCLKernelBase(const Worker& worker, OpenCLManager& manager, cl::Kernel& kernel) :
        manager_(manager), kernel_(std::make_shared<cl::Kernel>(kernel)), worker_ref_(worker) {
    name_ = kernel_->getInfo<CL_KERNEL_FUNCTION_NAME>();
}


std::unique_lock<std::mutex> OpenCLKernelBase::AcquireLock() {
    return std::unique_lock<std::mutex>(external_mutex_);
}

void OpenCLKernelBase::Run(const OpenCLDevice& device, bool block, const cl::NDRange& offset, const cl::NDRange& global,
    const cl::NDRange& local, std::unique_lock<std::mutex> external_lock) {

    cl_int err;
    cl::Event kernel_event;
    std::cerr << 1 << std::endl;
    auto& queue = device.GetQueue();
    
    {
        std::cerr << 2 << std::endl;
        std::lock_guard<std::mutex> guard(kernel_mutex_);

        std::cerr << 3 << std::endl;

        err = queue.GetRef().enqueueNDRangeKernel(*kernel_, offset, global, local, NULL, &kernel_event);
        std::cerr << 4 << std::endl;
        if (err != CL_SUCCESS) {
            throw OpenCLException(std::string(GET_FUNC)+": Failed to enqueue kernel '"+name_+"' for execution", err);
        }

        // If we were given an external lock we can "consume" it
        if (external_lock.owns_lock()) {
            external_lock.unlock();
        }
    }

    if (block) {
        std::cerr << 5 << std::endl;
        kernel_event.wait();
        std::cerr << 6 << std::endl;
    }
}

void OpenCLKernelBase::SetArg(unsigned int index, size_t size, const void* value) {
    cl_int err;
    
    // If we are passed something derived from GenericBuffer we know the backing reference will be properly handled
    err = kernel_->setArg(index, size, value);
    if (err != CL_SUCCESS) {
        throw OpenCLException(std::string(GET_FUNC)+"Unable to set parameter "+std::to_string(index)+" of a kernel", err);
    }
}

void OpenCLKernelBase::SetArg(unsigned int index, const cl::Memory& mem_obj) {
    cl_int err;
    
    err = kernel_->setArg(index, mem_obj);
    if (err != CL_SUCCESS) {
        throw OpenCLException(std::string(GET_FUNC)+"Unable to set parameter "+std::to_string(index)+" of a kernel", err);
    }
}

void OpenCLKernelBase::SetArg(unsigned int index, const cl::Buffer& buf_obj) {
    SetArg(index, (const cl::Memory&) buf_obj);
}

void OpenCLKernelBase::SetArg(unsigned int index, const cl::LocalSpaceArg& local_space) {
    cl_int err;
    
    err = kernel_->setArg(index, local_space);
    if (err != CL_SUCCESS) {
        throw OpenCLException(std::string(GET_FUNC)+"Unable to set parameter "+std::to_string(index)+" of a kernel", err);
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