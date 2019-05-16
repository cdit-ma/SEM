#ifndef OPENCLKERNELBASE_H
#define OPENCLKERNELBASE_H

#include "core/worker.h"

#include <memory>

namespace cl {
class Kernel;
class Memory;
class Buffer;
class NDRange;
struct LocalSpaceArg;
} // namespace cl

namespace Re {

namespace OpenCL {

class Device;
class Manager;

class KernelBase {
    public:
    void Run(const Device& device,
             bool block,
             const cl::NDRange& offset,
             const cl::NDRange& global,
             const cl::NDRange& local,
             std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>());

    void SetArg(unsigned int index, size_t size, const void* value);
    void SetArg(unsigned int index, const cl::Memory& mem_obj);
    void SetArg(unsigned int index, const cl::Buffer& buf_obj);
    void SetArg(unsigned int index, const cl::LocalSpaceArg& local_space);

    std::unique_lock<std::mutex> AcquireLock();

    std::string GetName() const;

    cl::Kernel GetBackingRef();

    protected:
    KernelBase(Manager& manager, cl::Kernel& kernel);
    ~KernelBase() noexcept = default;

    virtual void
    LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code);
    virtual void LogError(const Worker& worker, std::string function_name, std::string error_message);

    std::reference_wrapper<Manager> manager_;
    std::shared_ptr<cl::Kernel> kernel_;
    std::string name_;

    std::mutex kernel_mutex_;
    std::mutex external_mutex_;
};

} // namespace OpenCL

} // namespace Re

#endif