#include "device.h"

#include "manager.h"
#include "utilities.h"

#include <algorithm>

using namespace Re::OpenCL;

Device::Device(Manager& manager, cl::Device& device) :
    dev_(new cl::Device(device)),
    manager_(manager),
    name_(dev_->getInfo<CL_DEVICE_NAME>()),
    default_queue_(AddQueue("default"))
{
    // default_queue_ = std::make_shared<Queue>(manager, *this);
    valid_ = true;
}

const cl::Device& Device::GetRef() const { return *dev_; }

std::string Device::GetName() const { return name_; }

Queue& Device::AddQueue(const std::string& name)
{
    const auto&& emplace_result = queue_map_.emplace(name,
                                                     std::make_unique<Queue>(manager_, *this));
    // Check if emplace reported an insertion occurring
    if(emplace_result.second != true) {
        throw std::runtime_error("Failed to add Queue");
    }

    // Pull out the reference of the newly inserted queue and return
    return *((*(emplace_result.first)).second);
}

Queue& Device::GetQueue() const { return default_queue_; }

Queue& Device::GetQueue(const std::string& name) const
{
    auto&& map_itr = queue_map_.find(name);
    if(map_itr == queue_map_.end()) {
        throw OpenCLException("Attempting to get non-existent Queue: " + name,
                              CL_INVALID_COMMAND_QUEUE);
    }
    return *map_itr->second;
}

// May throw
void Device::LoadKernelsFromSource(const std::vector<std::string>& filenames)
{
    cl_int err;

    std::vector<Kernel> kernels;

    // Read, compile and link the Program from OpenCL code
    cl::Program::Sources sources = ReadOpenCLSourceCode(filenames);

    std::lock_guard<std::mutex> guard(kernel_list_mutex_);

    programs_.emplace_back(std::make_shared<cl::Program>(manager_.GetContext(), sources, &err));
    if(err != CL_SUCCESS) {
        throw OpenCLException("Unable to create OpenCL program from OpenCL source code", err);
    }
    cl::Program& new_program = *(programs_.back());

    // Get all devices with the same name
    std::vector<cl::Device> device_vec;
    for(const auto& other_dev : manager_.GetDevices()) {
        if(other_dev->GetName() == name_) {
            device_vec.emplace_back(other_dev->GetRef());
        }
    }
    err = new_program.build(device_vec);
    if(err != CL_SUCCESS) {
        throw OpenCLException("An error occurred while building an OpenCL program: \n"
                                  + new_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(*dev_),
                              err);
    }

    std::vector<cl::Kernel> new_kernels;
    err = new_program.createKernels(&new_kernels);
    if(err != CL_SUCCESS) {
        throw OpenCLException("An error occurred during the creation of OpenCL kernels from a "
                              "built program",
                              err);
    }

    std::vector<std::string> kernel_names;
    // std::vector<OpenCLDevice> all_devices = manager_.GetDevices(worker);
    for(const auto& other_dev : manager_.GetDevices()) {
        if(other_dev.get() != this && other_dev->GetName() == name_) {
            for(const Kernel& kernel : other_dev->GetKernels()) {
                kernel_names.push_back(kernel.GetName());
            }
        }
    }
    for(const auto& kernel_name : kernel_names) {
        new_kernels.emplace_back(new_program, kernel_name.c_str(), &err);
        if(err != CL_SUCCESS) {
            throw OpenCLException("An error occurred during the fallback operation where "
                                  "kernels are created individually",
                                  err);
        }
    }

    for(auto& kernel : new_kernels) {
        kernels_.emplace_back(new Kernel(manager_, kernel));
    }
}

// May throw
void Device::LoadKernelsFromBinary(const std::string& filename)
{
    cl_int err;

    std::vector<Kernel> kernels;
    std::vector<std::string> filenames;
    filenames.push_back(filename);
    cl::Program::Binaries binaries;
    try {
        binaries = ReadOpenCLBinaries(filenames);
    } catch(const std::exception& e) {
        throw OpenCLException(std::string("An error occurred while trying to read OpenCL "
                                          "binaries:\n")
                                  + e.what(),
                              CL_INVALID_BINARY);
    }

    std::vector<cl::Device> device_vec;
    device_vec.push_back(*dev_);
    std::vector<cl_int> binary_success;

    std::lock_guard<std::mutex> guard(kernel_list_mutex_);

    programs_.emplace_back(std::make_shared<cl::Program>(manager_.GetContext(), device_vec,
                                                         binaries, &binary_success, &err));
    if(err != CL_SUCCESS) {
        throw OpenCLException("Unable to create OpenCL program from OpenCL binary", err);
    }
    for(auto& result : binary_success) {
        if(result != CL_SUCCESS) {
            throw OpenCLException("An error occurred while loading an OpenCL binary", err);
        }
    }

    cl::Program& new_program = *(programs_.back());
    err = new_program.build(device_vec);
    if(err != CL_SUCCESS) {
        throw OpenCLException("An error occurred while building an OpenCL program: \n"
                                  + new_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(*dev_),
                              err);
    }

    std::vector<cl::Kernel> new_kernels;
    err = new_program.createKernels(&new_kernels);
    if(err != CL_SUCCESS) {
        throw OpenCLException("An error occurred during the creation of OpenCL kernels from a "
                              "built program",
                              err);
    }

    bool did_errors_occur = false;
    std::string error_string = "Errors occurred while adding kernels from binary to kernel list";

    for(auto& kernel : new_kernels) {
        std::string name = kernel.getInfo<CL_KERNEL_FUNCTION_NAME>();
        bool name_already_exists = false;
        for(const auto& k : kernels_) {
            if(k->GetName() == name) {
                name_already_exists = true;
            }
        }
        if(name_already_exists) {
            did_errors_occur = true;
            error_string += "Attempting to add kernel named " + name
                            + " when a kernel with that name already exists, skipping\n";
        } else {
            kernels_.emplace_back(new Kernel(manager_, kernel));
        }
    }

    if(did_errors_occur) {
        throw OpenCLException(error_string, CL_INVALID_KERNEL_NAME);
    }
}

const std::vector<std::reference_wrapper<Kernel>> Device::GetKernels() const
{
    std::lock_guard<std::mutex> guard(kernel_list_mutex_);
    std::vector<std::reference_wrapper<Kernel>> kernel_refs;
    for(const auto& kernel : kernels_) {
        kernel_refs.emplace_back(std::ref(*kernel));
    }
    return kernel_refs;
}

Kernel& Device::GetKernel(const std::string& name) const
{
    std::lock_guard<std::mutex> guard(kernel_list_mutex_);
    for(const auto& kernel : kernels_) {
        if(kernel->GetName() == name) {
            return *kernel;
        }
    }
    throw std::runtime_error("Device does not contain a kernel with name '" + name + "'");
}

void Device::LogError(const Worker& worker,
                      std::string function_name,
                      std::string error_message,
                      cl_int cl_error_code)
{
    LogOpenCLError(worker, "Device::" + function_name, error_message, cl_error_code);
}

void Device::LogError(const Worker& worker, std::string function_name, std::string error_message)
{
    LogOpenCLError(worker, "Device::" + function_name, error_message);
}
