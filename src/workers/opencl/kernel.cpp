
#include "kernel.hpp"
#include "manager.h"
#include "utilities.h"

using namespace Re::OpenCL;

Kernel::Kernel(Manager& manager, cl::Kernel& kernel) : KernelBase(manager, kernel) {}

void Kernel::LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code)
{
    LogOpenCLError(worker, "Kernel::" + function_name, error_message, cl_error_code);
}

void Kernel::LogError(const Worker& worker, std::string function_name, std::string error_message)
{
    LogOpenCLError(worker, "Kernel::" + function_name, error_message);
}