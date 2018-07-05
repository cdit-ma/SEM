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

#include "core/worker.h"

class OpenCLKernelBase {
public:

    OpenCLKernelBase(OpenCLKernelBase&& other) noexcept :
        manager_(other.manager_),
        kernel_(other.kernel_),
        worker_ref_(other.worker_ref_),
        name_(other.name_) {

    };
    OpenCLKernelBase& operator=(OpenCLKernelBase&& other) noexcept {};

    void Run(const OpenCLDevice& device, bool block, const cl::NDRange& offset,
        const cl::NDRange& global, const cl::NDRange& local, std::unique_lock<std::mutex> lock=std::unique_lock<std::mutex>());

    void SetArg(unsigned int index, size_t size, const void* value);
    void SetArg(unsigned int index, const cl::Memory& mem_obj);
    void SetArg(unsigned int index, const cl::Buffer& buf_obj);
    void SetArg(unsigned int index, const cl::LocalSpaceArg& local_space);

    std::unique_lock<std::mutex> AcquireLock();

    std::string GetName() const;

    cl::Kernel GetBackingRef();

protected:
    OpenCLKernelBase(const Worker& worker, OpenCLManager& manager, cl::Kernel& kernel);
    ~OpenCLKernelBase() noexcept = default;

    virtual void LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code);
    virtual void LogError(const Worker& worker, std::string function_name, std::string error_message);
    
    std::reference_wrapper<OpenCLManager> manager_;
    std::shared_ptr<cl::Kernel> kernel_;
    const Worker& worker_ref_;
    std::string name_;

    std::mutex kernel_mutex_;
    std::mutex external_mutex_;
};

#endif