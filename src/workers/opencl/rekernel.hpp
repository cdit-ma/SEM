#ifndef REKERNEL_H
#define REKERNEL_H

#include "openclmanager.h"
#include "openclutilities.h"
#include "core/worker.h"
#include "cl2.hpp"

class ReKernel {
public:
    ReKernel(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker);

    template <typename T0, typename... Ts>
    void SetArgs(T0 arg0, Ts... args);
    void Run(unsigned int gpu_num, bool block, const cl::NDRange& offset,
            const cl::NDRange& global, const cl::NDRange& local);

private:
    
    OpenCLManager& manager_;
    cl::Kernel& kernel_;
    Worker* worker_ref_;
    
    template <typename T0, typename... Ts>
    void SetArgsRecursive(unsigned int index, T0 arg0, Ts... args);
    template <typename T0>
    void SetArgsRecursive(unsigned int index, T0 arg0);

    void LogError(std::string function_name,
        std::string error_message,
        cl_int cl_error_code);
    void LogError(std::string function_name, std::string error_message);
    
};

ReKernel::ReKernel(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker) :
     manager_(manager), kernel_(kernel), worker_ref_(worker) {
    
}

template <typename T0, typename... Ts>
void ReKernel::SetArgs(T0 arg0, Ts... args) {
    std::cerr << "about to make the recursive call... " <<std::endl;
    SetArgsRecursive(0, arg0, args...);
}

void ReKernel::Run(unsigned int gpu_num, bool block, const cl::NDRange& offset,
        const cl::NDRange& global, const cl::NDRange& local) {
    cl::CommandQueue queue = manager_.GetQueues()[gpu_num];
    queue.enqueueNDRangeKernel(kernel_, offset, global, local);
}

template <typename T0, typename... Ts>
void ReKernel::SetArgsRecursive(unsigned int index, T0 arg0, Ts... args) {
    static_assert(!std::is_pointer<T0>::value, "SetArgs does not accept pointered types");
    std::cerr << "entering recursive call " <<std::endl;
    //std::cerr << arg0 << std::endl;
    cl_int err = kernel_.setArg(index, arg0);
    std::cerr << err <<std::endl;
    if (err != CL_SUCCESS) {
        LogError(__func__,
            "Unable to set parameter "+std::to_string(index)+" of kernel "+kernel_.getInfo<CL_KERNEL_FUNCTION_NAME>(),
            err);
            
        std::cerr << "arg size: " << ((cl::Buffer)arg0).getInfo<CL_MEM_SIZE>() <<std::endl;
    }
    std::cerr << "set arg " << index << std::endl;
    return SetArgsRecursive(index+1, args...);
}

template <typename T0>
void ReKernel::SetArgsRecursive(unsigned int index, T0 arg0) {
    std::cerr << "entering base call " <<std::endl;
    cl_int err = kernel_.setArg(index, arg0);
    if (err != CL_SUCCESS) {
        LogError(__func__,
            "Unable to set parameter "+std::to_string(index)+" of a kernel",
            err);
    }
    std::cerr << "set arg " << index << std::endl;
}


void ReKernel::LogError(std::string function_name, std::string error_message,
        cl_int cl_error_code) {
    LogOpenCLError(worker_ref_,
        "ReKernel::" + function_name,
        error_message,
        cl_error_code);
}

void ReKernel::LogError(std::string function_name, std::string error_message) {
    LogOpenCLError(worker_ref_,
        "ReKernel::" + function_name,
        error_message);
}

#endif