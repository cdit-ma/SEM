
#include "openclkernel.hpp"
#include "openclmanager.h"
#include "openclutilities.h"


OpenCLKernel::OpenCLKernel(const Worker& worker, OpenCLManager& manager, cl::Kernel& kernel) :
    OpenCLKernelBase(worker, manager, kernel) {}



void OpenCLKernel::LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code) {
    LogOpenCLError(worker,
        "OpenCLKernel::" + function_name,
        error_message,
        cl_error_code);
}

void OpenCLKernel::LogError(const Worker& worker, std::string function_name, std::string error_message) {
    LogOpenCLError(worker,
        "OpenCLKernel::" + function_name,
        error_message);
}