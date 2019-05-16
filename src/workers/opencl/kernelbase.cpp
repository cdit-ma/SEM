
#include "kernelbase.h"
#include "manager.h"

#include "openclexception.h"
#include "utilities.h"

#include <iostream>

#include <mutex>

using namespace Re::OpenCL;

KernelBase::KernelBase(Manager& manager, cl::Kernel& kernel) :
    manager_(manager),
    kernel_(std::make_shared<cl::Kernel>(kernel))
{
    name_ = kernel_->getInfo<CL_KERNEL_FUNCTION_NAME>();
}

std::unique_lock<std::mutex> KernelBase::AcquireLock() { return std::unique_lock<std::mutex>(external_mutex_); }

void KernelBase::Run(const Device& device,
                     bool block,
                     const cl::NDRange& offset,
                     const cl::NDRange& global,
                     const cl::NDRange& local,
                     std::unique_lock<std::mutex> external_lock)
{
    cl::Event kernel_event;
    auto& queue = device.GetQueue();

    {
        std::lock_guard<std::mutex> guard(kernel_mutex_);

        cl_int err = queue.GetRef().enqueueNDRangeKernel(*kernel_, offset, global, local, NULL, &kernel_event);
        if(err != CL_SUCCESS) {
            throw OpenCLException(std::string(GET_FUNC) + ": Failed to enqueue kernel '" + name_ + "' for execution",
                                  err);
        }

        // If we were given an external lock we can "consume" it
        if(external_lock.owns_lock()) {
            external_lock.unlock();
        }
    }

    if(block) {
        cl_int status = kernel_event.wait();
        if(status != CL_SUCCESS) {
            throw OpenCLException("An error occurred while waiting for a blocking kernel run", status);
        }
    }
}

void KernelBase::SetArg(unsigned int index, size_t size, const void* value)
{
    cl_int err;

    // If we are passed something derived from GenericBuffer we know the backing reference will be properly handled
    err = kernel_->setArg(index, size, value);
    if(err != CL_SUCCESS) {
        throw OpenCLException(
            std::string(GET_FUNC) + "Unable to set parameter " + std::to_string(index) + " of a kernel", err);
    }
}

void KernelBase::SetArg(unsigned int index, const cl::Memory& mem_obj)
{
    cl_int err;

    err = kernel_->setArg(index, mem_obj);
    if(err != CL_SUCCESS) {
        throw OpenCLException(
            std::string(GET_FUNC) + "Unable to set parameter " + std::to_string(index) + " of a kernel", err);
    }
}

void KernelBase::SetArg(unsigned int index, const cl::Buffer& buf_obj) { SetArg(index, (const cl::Memory&)buf_obj); }

void KernelBase::SetArg(unsigned int index, const cl::LocalSpaceArg& local_space)
{
    cl_int err;

    err = kernel_->setArg(index, local_space);
    if(err != CL_SUCCESS) {
        throw OpenCLException(
            std::string(GET_FUNC) + "Unable to set parameter " + std::to_string(index) + " of a kernel", err);
    }
}

std::string KernelBase::GetName() const { return name_; }

cl::Kernel KernelBase::GetBackingRef() { return *kernel_; }

void KernelBase::LogError(const Worker& worker,
                          std::string function_name,
                          std::string error_message,
                          cl_int cl_error_code)
{
    LogOpenCLError(worker, "KernelBase::" + function_name, error_message, cl_error_code);
}

void KernelBase::LogError(const Worker& worker, std::string function_name, std::string error_message)
{
    LogOpenCLError(worker, "KernelBase::" + function_name, error_message);
}