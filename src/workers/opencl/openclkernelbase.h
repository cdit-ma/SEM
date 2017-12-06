#ifndef OPENCLKERNELBASE_H
#define OPENCLKERNELBASE_H

#include <memory>

class OpenCLDevice;
class OpenCLManager;

namespace cl {
    class Kernel;
    class Memory;
    class Buffer;
    class NDRange;
    struct LocalSpaceArg;
}

//#include "openclmanager.h"
#include "core/worker.h"

class OpenCLKernelBase {
public:

    bool Run(const OpenCLDevice& device, bool block, const cl::NDRange& offset,
        const cl::NDRange& global, const cl::NDRange& local);

    bool SetArg(unsigned int index, size_t size, const void* value);
    bool SetArg(unsigned int index, const cl::Memory& mem_obj);
    bool SetArg(unsigned int index, const cl::Buffer& buf_obj);
    bool SetArg(unsigned int index, const cl::LocalSpaceArg& local_space);

    std::string GetName() const;

    cl::Kernel GetBackingRef();

protected:
    OpenCLKernelBase(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker);

    virtual void LogError(std::string function_name, std::string error_message, int cl_error_code);
    virtual void LogError(std::string function_name, std::string error_message);
    
    OpenCLManager& manager_;
    std::shared_ptr<cl::Kernel> kernel_;
    Worker* worker_ref_;
    std::string name_;


};

#endif