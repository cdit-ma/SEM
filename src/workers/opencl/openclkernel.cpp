
#include "openclkernel.hpp"
#include "openclmanager.h"


OpenCLKernel::OpenCLKernel(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker) :
    OpenCLKernelBase(manager, kernel, worker) {}



void OpenCLKernel::LogError(std::string function_name, std::string error_message,
    cl_int cl_error_code) {
    LogOpenCLError(worker_ref_,
    "OpenCLKernel::" + function_name,
    error_message,
    cl_error_code);
}

void OpenCLKernel::LogError(std::string function_name, std::string error_message) {
    LogOpenCLError(worker_ref_,
    "OpenCLKernel::" + function_name,
    error_message);
}