
#include "openclutilities.h"
#include "openclmanager.h"
#include "openclkernel.hpp"
#include "opencldevice.h"

#include <core/worker.h>

#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>

#include <unordered_set>

std::mutex reference_list_mutex_;
std::unordered_map<unsigned int, std::unique_ptr<OpenCLManager> > OpenCLManager::reference_map_;
std::vector<cl::Platform> OpenCLManager::platform_list_;

/************************************************************************/
/* Static functions                                                     */
/************************************************************************/

OpenCLManager* OpenCLManager::GetReferenceByPlatformID(const Worker& worker, int platform_id) {
	
    std::lock_guard<std::mutex> guard(reference_list_mutex_);

	// If we haven't initialized the length of the reference list yet do so now
	//if (reference_map_.empty()) {
		/*cl_int errCode = cl::Platform::get(&platform_list_);
		if (errCode != CL_SUCCESS) {
			LogError(worker,
					std::string(__func__),
					"Failed to retrieve the list of OpenCL platforms",
					errCode);
			return NULL;
		}*/
		GetPlatforms(worker);
		//reference_list_.resize(platform_list_.size());
	//}

	// Check that the specified platform index isn't out of bounds
	if (platform_id >= platform_list_.size() || platform_id < 0) {
	//if (!reference_map_.count(platform_id)) 
		LogError(worker,
			std::string(__func__),
			"platform_id is out of bounds (" + std::to_string(platform_id) + ")");
		return NULL;
	}
	
	// If we haven't created an OpenCLManager for the specified platform do so now
	if (reference_map_.count(platform_id) == 0) {
		OpenCLManager* new_manager = new OpenCLManager(worker, platform_list_[platform_id]);

		if (!new_manager->IsValid()) {
			LogError(worker,
					std::string(__func__),
					"Unable to create OpenCLManager for platform " + std::to_string(platform_id));
			delete new_manager;
			return NULL;
		}

		reference_map_.emplace(platform_id,  std::move(std::unique_ptr<OpenCLManager>(new_manager)));
		//reference_list_.at(platform_id).swap(std::move(std::unique_ptr<OpenCLManager>(new_manager)));// = std::move();
	}

	// Return the relevant reference
	return reference_map_.at(platform_id).get();
	//return reference_list_[platform_id].get();

}

OpenCLManager* OpenCLManager::GetReferenceByPlatformName(const Worker& worker, std::string platform_name) {
	
    std::lock_guard<std::mutex> guard(reference_list_mutex_);
	cl_int err;

	// If we haven't initialized the length of the reference list yet do so now
	//if (reference_list_.empty()) {
		GetPlatforms(worker);
		//reference_list_.resize(platform_list_.size(), NULL);
	//}

	// Find the index of the platform with the requested name
	unsigned int platform_id = 0;
	bool found_match = false;
	for (auto& platform : platform_list_) {
		std::string name = platform.getInfo<CL_PLATFORM_NAME>(&err);
		if (err != CL_SUCCESS) {
			throw OpenCLException("Unable to query OpenCL platform for name", err);
		}
		if (name == platform_name) {
			found_match = true;
			break;
		}
		platform_id++;
	}
	if (!found_match) {
		throw std::runtime_error("Unable to find an OpenCL platform with the name " + platform_name);
	}

	
	// If we haven't created an OpenCLManager for the specified platform do so now
	if (reference_map_.count(platform_id) == 0) {
		OpenCLManager* new_manager = new OpenCLManager(worker, platform_list_[platform_id]);

		if (!new_manager->IsValid()) {
			LogError(worker,
					std::string(__func__),
					"Unable to create OpenCLManager for platform " + std::to_string(platform_id));
			delete new_manager;
			return NULL;
		}

		reference_map_.emplace(platform_id, std::move(std::unique_ptr<OpenCLManager>(new_manager)));
		//reference_list_.at(platform_id).swap(std::unique_ptr<OpenCLManager>(new_manager));
	}

	// Return the relevant reference
	return reference_map_.at(platform_id).get();
	//return reference_list_[platform_id].get();

}

const std::vector<cl::Platform> OpenCLManager::GetPlatforms(const Worker& worker) {
	cl_int err_code = cl::Platform::get(&platform_list_);
	if (err_code != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"Failed to retrieve the list of OpenCL platforms",
			err_code);
		return std::vector<cl::Platform>();
	}

	return platform_list_;
}

/************************************************************************/
/* Public Functions                                                     */
/************************************************************************/

/**
 * Construct an OpenCLManager for the specified OpenCL platform, creating a context
 * to hold all of its devices and then attempt to load pre-compiled binaries for
 * each device.
 */
OpenCLManager::OpenCLManager(const Worker& worker, cl::Platform& platform_) : platform_(platform_) {
	cl_int err;

	// Manager shouldn't be marked as valid until creation has completed successfully
	valid_ = false;

	platform_name_ = platform_.getInfo<CL_PLATFORM_NAME>(&err);
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"Unable to retrieve platform name",
			err);

	}

	// Flag the platofmr as conatianing FPGAs if it constains the string FPGA in the name
    std::string lower_name(platform_name_);
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
    if (lower_name.find("fpga") != std::string::npos) {
        is_fpga_ = true;
    }

	// Pull out all OpenCL devices in the platform (don't distinguish between CPU, GPU, etc)
	std::vector<cl::Device> devices;
	err = platform_.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"Failed to retrieve the list of devices for platform " + platform_name_,
			err);
	}

	if (devices.size() == 0)
	{
		LogError(worker,
			std::string(__func__),
			"Unable to find any devices for the given OpenCL platform (" + platform_name_ + ")");
		return;
	}

	// Create a context from the collected devices
	context_ = std::unique_ptr<cl::Context>(new cl::Context(devices, NULL, NULL, NULL, &err));
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"Unable to create an OpenCL context (" + platform_name_ + ")",
			err);
		return;
	}

	// Create a wrapped representation for each of the devices
	for(auto& d : devices) {
		device_list_.emplace_back(new OpenCLDevice(worker, *this, d));
	}

	// Fetch all of the pre-compiled binaries
	LoadAllBinaries(worker);

	// On FPGA platforms it is expected that the FFT kernels have also been precompiled
    if (is_fpga_) {
        for (const auto& dev : device_list_) {
            try {
                dev->LoadKernelsFromBinary(worker, "fft1d.aocx");
            } catch (const OpenCLException& ocle) {
				throw OpenCLException("Failed to load FFT implementation for FPGA: "+std::string(ocle.what()), ocle.ErrorCode());
            }
        }
            
    }

	// Mark the manager as having been properly initialized
	valid_ = true;
}

OpenCLManager::~OpenCLManager() {
	size_t leaked_memory = 0;
	size_t num_unfreed_buffers = buffer_store_.size();
	for (auto& unfreed_buffer_pair : buffer_store_) {
		//std::cerr << unfreed_buffer_pair.second.get() << std::endl;
		//std::cerr << unfreed_buffer_pair.second->IsValid() << std::endl;
		//std::cerr << "deleting buffer at mem location " << unfreed_buffer_pair.second << std::endl;
		//delete unfreed_buffer_pair.second;
		leaked_memory += unfreed_buffer_pair.second;
	}

	if (!buffer_store_.empty()) {
		std::cout << "OpenCLManager was destroyed before all buffers were released, "
				+std::to_string(leaked_memory)+" bytes automatically freed across "
				+std::to_string(num_unfreed_buffers)+" buffers" << std::endl;
		
		Log(Severity::WARNING)
			.Class("OpenCLManager")
			.Func(__func__)
			.Msg("OpenCLManager was destroyed before all buffers were released, "
				+std::to_string(leaked_memory)+" bytes automatically freed across "
				+std::to_string(num_unfreed_buffers)+" buffers");
	}

	buffer_store_.clear();

#ifdef BUILD_TEST
	std::cerr << buffer_store_.size() << " buffers haven't been deallocated after OpenCL testing has finished" << std::endl;	
#endif
}

// TODO: Handle the !valid_ case
const cl::Context& OpenCLManager::GetContext() const {
	return *context_;
}

// TODO: Handle the !valid_ case
std::string OpenCLManager::GetPlatformName() const {
	return platform_name_;
}

// TODO: Handle the !valid_ case
const std::vector<std::unique_ptr<OpenCLDevice> > & OpenCLManager::GetDevices(const Worker& worker) {
	return device_list_;
}

// TODO: Handle the !valid_ case
const std::vector<std::shared_ptr<cl::CommandQueue> > OpenCLManager::GetQueues() const {
	return queues_;
}

// May throw
const std::vector<OpenCLKernel> OpenCLManager::CreateKernels(const Worker& worker, const std::vector<std::string>& filenames) {
    std::lock_guard<std::mutex> guard(opencl_resource_mutex_);
	std::vector<OpenCLKernel> kernels;

	// Read, compile and link the Program from OpenCL code
	cl::Program::Sources sources = ReadOpenCLSourceCode(filenames);

	for (const auto& device : device_list_) {
		device->LoadKernelsFromSource(worker, filenames);
	}

	return kernels;
}

bool OpenCLManager::IsValid() const {
	return valid_;
}

bool OpenCLManager::IsFPGA() const {
    return is_fpga_;
}

/************************************************************************/
/* Private Functions                                                    */
/************************************************************************/


int OpenCLManager::TrackBuffer(const Worker& worker, const OpenCLBufferBase& buffer){
    std::lock_guard<std::mutex> guard(opencl_buffer_mutex_);
	
	//try to retrieve a new buffer ID
	auto buffer_id  = buffer_id_count_++;
	if (buffer_id == invalid_buffer_id_) {
		buffer_id = buffer_id_count_++;
	}

	if (!buffer_store_.count(buffer_id)){
    	std::lock_guard<std::mutex> guard(opencl_resource_mutex_);
		buffer_store_.insert(std::make_pair(buffer_id, buffer.GetByteSize()));
	} else {
		LogError(worker, __func__, "Got Duplicate Buffer ID: " + std::to_string(buffer_id));
		buffer_id = invalid_buffer_id_;
	}
	return buffer_id;
}

void OpenCLManager::UntrackBuffer(int buffer_id) {
    std::lock_guard<std::mutex> guard(opencl_buffer_mutex_);
	buffer_store_.erase(buffer_id);
}

bool OpenCLManager::LoadAllBinaries(const Worker& worker) {

    bool did_all_succeed = true;
    for (const auto& device : device_list_) {
        std::string dev_name = SanitisePathString(device->GetName()).substr(0, 15);
		std::string plat_name = SanitisePathString(platform_name_).substr(0, 15);

		std::string filename = plat_name+"-"+dev_name+".clbin";

        std::string binary_path = GetSourcePath("binaries/"+filename);
        bool success = false;
		try {
			success = device->LoadKernelsFromBinary(worker, binary_path);
		} catch (const std::exception& e) {
			LogError(worker, __func__,
                "Failed to load binary for device "+dev_name+":\n"+e.what());
            did_all_succeed = false;
		}
        if (!success) {
            LogError(worker, __func__,
                "Note: Failed to load binary for device "+dev_name+", will attempt to compile kernels on demand at runtime");
            did_all_succeed = false;
        } else {
			std::cout << "finished reading precompiled binary for " << dev_name << ", list of avaialble kernels: " << std::endl;
			for (OpenCLKernel& kernel : device->GetKernels()) {
				std::cout << " - " << kernel.GetName() << std::endl;
			}
		}
    }
    return did_all_succeed;
}

void OpenCLManager::LogError(const Worker& worker,
							std::string function_name,
							std::string error_message,
							int cl_error_code)
{
	
	LogOpenCLError(worker,
		"OpenCLManager::" + function_name,
		error_message,
		cl_error_code);
}

void OpenCLManager::LogError(const Worker& worker,
							std::string function_name,
							std::string error_message)
{
	LogOpenCLError(worker,
		"OpenCLManager::" + function_name,
		error_message);
}
