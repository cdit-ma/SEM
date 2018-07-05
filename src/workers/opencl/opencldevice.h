#ifndef OPENCLDEVICE_H
#define OPENCLDEVICE_H

#include <memory>
#include <string>
#include <list>
#include <mutex>

#include <core/worker.h>

#include "openclkernel.hpp"
#include "openclqueue.h"

//class OpenCLKernel;
class OpenCLManager;

namespace cl {
    class Device;
	class CommandQueue;
    class Program;
}

class OpenCLDevice {
public:
    OpenCLDevice(const Worker& worker, OpenCLManager& manager, cl::Device& device);
    ~OpenCLDevice() noexcept = default;

    const cl::Device& GetRef() const;
    std::string GetName() const;
    // cl::CommandQueue& GetQueue() const;
    OpenCLQueue& GetQueue() const;

    bool LoadKernelsFromSource(const Worker& worker, const std::vector<std::string>& filenames);
    bool LoadKernelsFromBinary(const Worker& worker, const std::string& filename);
    const std::vector<std::reference_wrapper<OpenCLKernel> > GetKernels();
    //OpenCLKernel& GetKernel(const std::string& kernel_name) const;

private:
    virtual void LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code);
    virtual void LogError(const Worker& worker, std::string function_name, std::string error_message);

    std::unique_ptr<cl::Device> dev_;
    OpenCLManager& manager_;
    std::shared_ptr<OpenCLQueue> queue_;
    std::string name_;

    std::list<std::shared_ptr<cl::Program> > programs_;
    std::list<std::unique_ptr<OpenCLKernel> > kernels_;

    std::mutex kernel_list_mutex_;

    bool valid_ = false;
    int err_;
};

#endif