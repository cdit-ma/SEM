#ifndef OPENCLKERNELBASE_H
#define OPENCLKERNELBASE_H


class OpenCLManager;

//#include "openclmanager.h"
#include "openclutilities.h"
#include "core/worker.h"

class OpenCLKernelBase {
public:

    bool Run(unsigned int gpu_num, bool block, const cl::NDRange& offset,
        const cl::NDRange& global, const cl::NDRange& local);

    std::string GetName() const;

    cl::Kernel GetBackingRef();

protected:
    OpenCLKernelBase(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker);

    virtual void LogError(std::string function_name, std::string error_message, cl_int cl_error_code);
    virtual void LogError(std::string function_name, std::string error_message);
    
    OpenCLManager& manager_;
    cl::Kernel kernel_;
    Worker* worker_ref_;
    std::string name_;


};

#endif