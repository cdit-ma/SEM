#ifndef OPENCLDEVICE_H
#define OPENCLDEVICE_H

#include <memory>
#include <string>

#include <core/worker.h>

#include "openclkernel.hpp"

//class OpenCLKernel;
class OpenCLManager;

namespace cl {
    class Device;
	class CommandQueue;
    class Program;
}

class OpenCLDevice {
public:
    OpenCLDevice(OpenCLManager& manager, cl::Device& device, const Worker& worker);

    const cl::Device& GetRef() const;
    std::string GetName() const;
    const cl::CommandQueue& GetQueue() const;

    bool LoadKernelsFromSource(const std::vector<std::string>& filenames, Worker& worker);
    bool LoadKernelsFromBinary(const std::vector<std::string>& filenames, Worker& worker);
    const std::vector<std::reference_wrapper<OpenCLKernel> > GetKernels();
    OpenCLKernel& GetKernel(const std::string& kernel_name) const;

private:
    virtual void LogError(const Worker& worker_ref, std::string function_name, std::string error_message, int cl_error_code);
    virtual void LogError(const Worker& worker_ref, std::string function_name, std::string error_message);

    // /std::unique_ptr<cl::Device> device;
    std::unique_ptr<cl::Device> dev_;
    OpenCLManager& manager_;
    std::unique_ptr<cl::CommandQueue> queue_;
    std::string name_;

    std::vector<cl::Program> programs_;
    std::vector<OpenCLKernel> kernels_;

    bool valid_ = false;
    int err_;
};

#endif