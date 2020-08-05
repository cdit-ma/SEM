
#include "manager.h"
#include "device.h"
#include "kernel.hpp"
#include "utilities.h"

#include "opencl_worker.h"

#include "worker.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#include <unordered_set>

using namespace Re::OpenCL;

const std::set<std::string> known_fpga_platforms = {"Intel(R) FPGA SDK for OpenCL(TM)"};

std::mutex reference_list_mutex_;
std::unordered_map<unsigned int, std::unique_ptr<Manager>> Manager::reference_map_;
std::vector<cl::Platform> Manager::platform_list_;

/************************************************************************/
/* Static functions                                                     */
/************************************************************************/

Manager* Manager::GetReferenceByPlatformID(int platform_id, Worker* worker_ref)
{
    std::lock_guard<std::mutex> guard(reference_list_mutex_);

    UpdatePlatformList();

    // Check that the specified platform index isn't out of bounds
    if(platform_id >= platform_list_.size() || platform_id < 0) {
        throw OpenCLException("platform_id is out of bounds (" + std::to_string(platform_id) + ")",
                              CL_INVALID_PLATFORM);
        return NULL;
    }

    // If we haven't created an Manager for the specified platform do so now
    if(reference_map_.count(platform_id) == 0) {
        auto new_manager = std::unique_ptr<Manager>(new Manager(platform_list_.at(platform_id), worker_ref));

        if(!new_manager->IsValid()) {
            throw OpenCLException("Unable to create Manager for platform " + std::to_string(platform_id),
                                  CL_INVALID_PLATFORM);
        }

        reference_map_.emplace(platform_id, std::move(new_manager));
    }

    // Return the relevant reference
    return reference_map_.at(platform_id).get();
}

Manager* Manager::GetReferenceByPlatformName(std::string platform_name, Worker* worker_ref)
{
    std::lock_guard<std::mutex> guard(reference_list_mutex_);
    cl_int err;

    UpdatePlatformList();

    // Find the index of the platform with the requested name
    unsigned int platform_id = 0;
    bool found_match = false;
    for(auto& platform : platform_list_) {
        std::string name = platform.getInfo<CL_PLATFORM_NAME>(&err);
        if(err != CL_SUCCESS) {
            throw OpenCLException("Unable to query OpenCL platform for name", err);
        }
        if(name == platform_name) {
            found_match = true;
            break;
        }
        platform_id++;
    }
    if(!found_match) {
        throw std::runtime_error("Unable to find an OpenCL platform with the name " + platform_name);
    }

    // If we haven't created an Manager for the specified platform do so now
    if(reference_map_.count(platform_id) == 0) {
        auto new_manager = std::unique_ptr<Manager>(new Manager(platform_list_.at(platform_id), worker_ref));

        if(!new_manager->IsValid()) {
            throw OpenCLException("Unable to create Manager for platform " + std::to_string(platform_id),
                                  CL_INVALID_PLATFORM);
        }

        reference_map_.emplace(platform_id, std::move(new_manager));
    }

    // Return the relevant reference
    return reference_map_.at(platform_id).get();
}

const std::vector<cl::Platform> Manager::GetPlatforms()
{
    UpdatePlatformList();
    return platform_list_;
}

void Manager::UpdatePlatformList()
{
    cl_int err_code = cl::Platform::get(&platform_list_);
    if(err_code != CL_SUCCESS) {
        throw OpenCLException("Failed to retrieve the list of OpenCL platforms", err_code);
    }
}

/************************************************************************/
/* Public Functions                                                     */
/************************************************************************/

/**
 * Construct an Manager for the specified OpenCL platform, creating a context
 * to hold all of its devices and then attempt to load pre-compiled binaries for
 * each device.
 */
Manager::Manager(cl::Platform& platform_, Worker* worker_ref) : platform_(platform_)
{
    cl_int err;

    // Manager shouldn't be marked as valid until creation has completed successfully
    valid_ = false;

    platform_name_ = platform_.getInfo<CL_PLATFORM_NAME>(&err);
    if(err != CL_SUCCESS) {
        throw OpenCLException("Unable to retrieve platform name", err);
    }

    // Flag the platform as conatianing FPGAs if it contains the string FPGA in the name
    /*std::string lower_name(platform_name_);
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
    if(lower_name.find("fpga") != std::string::npos) {
        is_fpga_ = true;
    }*/
    if(known_fpga_platforms.count(platform_name_) > 0) {
        is_fpga_ = true;
    }

    // Pull out all OpenCL devices in the platform (don't distinguish between CPU, GPU, etc)
    std::vector<cl::Device> devices;
    err = platform_.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if(err != CL_SUCCESS) {
        throw OpenCLException("Failed to retrieve the list of devices for platform " + platform_name_, err);
    }

    if(devices.size() == 0) {
        throw OpenCLException("Unable to find any devices for the given OpenCL platform (" + platform_name_ + ")",
                              CL_DEVICE_NOT_FOUND);
    }

    // Create a context from the collected devices
    context_ = std::unique_ptr<cl::Context>(new cl::Context(devices, NULL, NULL, NULL, &err));
    if(err != CL_SUCCESS) {
        throw OpenCLException("Unable to create an OpenCL context (" + platform_name_ + ")", err);
    }

    // Create a wrapped representation for each of the devices
    for(auto& d : devices) {
        device_list_.emplace_back(new Device(*this, d));
    }

    // Fetch all of the pre-compiled binaries
    try {
        LoadAllBinaries();
    } catch(const OpenCLException& ocle) {
        if(worker_ref != nullptr) {
            worker_ref->Log(std::string(__func__), Logger::WorkloadEvent::WARNING, -1, ocle.what());
        }
    }

    // On FPGA platforms it is expected that the FFT kernels have also been precompiled
    /*if(is_fpga_) {
        for(const auto& dev : device_list_) {
            try {
                dev->LoadKernelsFromBinary("fft1d.aocx");
            } catch(const OpenCLException& ocle) {
                throw OpenCLException(std::string("Failed to load FFT implementation for FPGA: ") + ocle.what(),
                                      ocle.ErrorCode());
            }
        }
    }*/

    // Mark the manager as having been properly initialized
    valid_ = true;
}

Manager::~Manager()
{
    size_t leaked_memory = 0;
    size_t num_unfreed_buffers = buffer_store_.size();
    for(auto& unfreed_buffer_pair : buffer_store_) {
        leaked_memory += unfreed_buffer_pair.second;
    }

    if(!buffer_store_.empty()) {
        std::cout << "Manager was destroyed before all buffers were released, " + std::to_string(leaked_memory)
                         + " bytes automatically freed across " + std::to_string(num_unfreed_buffers) + " buffers"
                  << std::endl;
    }

    buffer_store_.clear();

#ifdef BUILD_TEST
    std::cerr << buffer_store_.size() << " buffers haven't been deallocated after OpenCL testing has finished"
              << std::endl;
#endif
}

// TODO: Handle the !valid_ case
const cl::Context& Manager::GetContext() const { return *context_; }

// TODO: Handle the !valid_ case
std::string Manager::GetPlatformName() const { return platform_name_; }

// TODO: Handle the !valid_ case
const std::vector<std::unique_ptr<Device>>& Manager::GetDevices() { return device_list_; }

// TODO: Handle the !valid_ case
// const std::vector<std::shared_ptr<cl::CommandQueue>> Manager::GetQueues() const { return queues_; }

// May throw
const std::vector<Kernel> Manager::CreateKernels(const std::vector<std::string>& filenames)
{
    std::lock_guard<std::mutex> guard(opencl_resource_mutex_);
    std::vector<Kernel> kernels;

    // Read, compile and link the Program from OpenCL code
    cl::Program::Sources sources = ReadOpenCLSourceCode(filenames);

    for(const auto& device : device_list_) {
        device->LoadKernelsFromSource(filenames);
    }

    return kernels;
}

bool Manager::IsValid() const { return valid_; }

bool Manager::IsFPGA() const { return is_fpga_; }

/************************************************************************/
/* Private Functions                                                    */
/************************************************************************/

int Manager::TrackBuffer(const BufferBase& buffer)
{
    std::lock_guard<std::mutex> guard(opencl_buffer_mutex_);

    // try to retrieve a new buffer ID
    auto buffer_id = buffer_id_count_++;
    if(buffer_id == invalid_buffer_id_) {
        buffer_id = buffer_id_count_++;
    }

    if(!buffer_store_.count(buffer_id)) {
        std::lock_guard<std::mutex> guard(opencl_resource_mutex_);
        buffer_store_.insert(std::make_pair(buffer_id, buffer.GetByteSize()));
        return buffer_id;
    } else {
        throw OpenCLException("Got Duplicate Buffer ID: " + std::to_string(buffer_id), CL_INVALID_MEM_OBJECT);
        buffer_id = invalid_buffer_id_;
    }
}

void Manager::UntrackBuffer(int buffer_id)
{
    std::lock_guard<std::mutex> guard(opencl_buffer_mutex_);
    buffer_store_.erase(buffer_id);
}

bool Manager::LoadAllBinaries()
{
    bool did_all_succeed = true;
    std::string failure_message = "Errors occurred while attempting to load all binaries:\n";

    for(const auto& device : device_list_) {
        std::string dev_name = SanitisePathString(device->GetName()).substr(0, 15);
        std::string plat_name = SanitisePathString(platform_name_).substr(0, 15);

        std::string filename = plat_name + "-" + dev_name + ".clbin";

        std::string binary_path = GetSourcePath("binaries/" + filename);
        try {
            device->LoadKernelsFromBinary(binary_path);
        } catch(const std::exception& e) {
            failure_message +=
                "Note: Unable to load pre-compiled binary for device " + dev_name + "due to " + e.what() + '\n';
            did_all_succeed = false;
        }
    }
    if(!did_all_succeed) {
        throw OpenCLException(failure_message, CL_COMPILE_PROGRAM_FAILURE);
    }
    return true;
}

void Manager::LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code)
{
    LogOpenCLError(worker, "Manager::" + function_name, error_message, cl_error_code);
}

void Manager::LogError(const Worker& worker, std::string function_name, std::string error_message)
{
    LogOpenCLError(worker, "Manager::" + function_name, error_message);
}

void Manager::LogMessage(const Worker& worker, std::string function_name, std::string message)
{
    LogOpenCLMessage(worker, "Manager::" + function_name, message);
}