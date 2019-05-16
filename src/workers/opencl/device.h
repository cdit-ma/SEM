#ifndef RE_OPENCL_DEVICE_H
#define RE_OPENCL_DEVICE_H

#include <list>
#include <memory>
#include <mutex>
#include <string>

#include <core/worker.h>

#include "kernel.hpp"
#include "queue.h"

namespace cl {
class Device;
class CommandQueue;
class Program;
} // namespace cl

namespace Re {

namespace OpenCL {

class Manager;

class Device {
    public:
    Device(Manager& manager, cl::Device& device);
    ~Device() noexcept = default;

    const cl::Device& GetRef() const;
    std::string GetName() const;
    Queue& AddQueue(const std::string& name);
    Queue& GetQueue() const;
    Queue& GetQueue(const std::string& name) const;

    void LoadKernelsFromSource(const std::vector<std::string>& filenames);
    void LoadKernelsFromBinary(const std::string& filename);
    const std::vector<std::reference_wrapper<Kernel>> GetKernels();
    Kernel& GetKernel(const std::string& name) const;

    private:
    virtual void
    LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code);
    virtual void LogError(const Worker& worker, std::string function_name, std::string error_message);

    std::unique_ptr<cl::Device> dev_;
    Manager& manager_;
    std::map<std::string, std::unique_ptr<Queue>> queue_map_;
    Queue& default_queue_; // Must be declared after queue_map_ due to init list ordering dependencies
    std::string name_;

    std::list<std::shared_ptr<cl::Program>> programs_;
    std::list<std::unique_ptr<Kernel>> kernels_;

    std::mutex kernel_list_mutex_;

    bool valid_ = false;
    int err_;
};

} // namespace OpenCL

} // namespace Re

#endif // RE_OPENCL_DEVICE_H