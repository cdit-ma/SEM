
#include "openclutilities.h"
#include "openclmanager.h"
#include "openclkernel.hpp"
#include "opencldevice.h"

#include <core/worker.h>

#include <string>
#include <fstream>
#include <sstream>

#include <unordered_set>

std::vector<OpenCLManager*> OpenCLManager::reference_list_;
std::vector<cl::Platform> OpenCLManager::platform_list_;


/*std::vector<OpenCLDevice> Device2Value(const std::vector<std::shared_ptr<OpenCLDevice> > device_list ){
	std::vector<OpenCLDevice> devices;
	for (auto d : device_list) {
		devices.push_back(*d);
	}
	return devices;
}*/

/************************************************************************/
/* Static functions                                                     */
/************************************************************************/


OpenCLManager* OpenCLManager::GetReferenceByPlatform(const Worker& worker, int platformID) {
	// If we haven't initialized the length of the reference list yet do so now
	if (reference_list_.empty()) {
		cl_int errCode = cl::Platform::get(&platform_list_);
		if (errCode != CL_SUCCESS) {
			LogError(worker,
					std::string(__func__),
					"Failed to retrieve the list of OpenCL platforms",
					errCode);
			return NULL;
		}

		reference_list_.resize(platform_list_.size(), NULL);
	}

	// Check that the specified platform index isn't out of bounds
	if (platformID >= reference_list_.size()) {
		LogError(worker,
				std::string(__func__),
				"PlatformID is out of bounds (" + std::to_string(platformID) + ")");
		return NULL;
	}
	
	// If we haven't created an OpenCLManager for the specified platform do so now
	if (reference_list_[platformID] == NULL) {
		OpenCLManager* newManager = new OpenCLManager(worker, platform_list_[platformID]);

		if (!newManager->IsValid()) {
			LogError(worker,
					std::string(__func__),
					"Unable to create OpenCLManager for platform " + std::to_string(platformID));
			delete newManager;
			return NULL;
		}

		reference_list_[platformID] = newManager;
	}

	// Return the relevant reference
	return reference_list_[platformID];

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
	context_ = new cl::Context(devices, NULL, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"Unable to create an OpenCL context (" + platform_name_ + ")",
			err);
		return;
	}

	// Create a representation for each of the devices
	for(auto& d : devices) {
		device_list_.emplace_back(worker, *this, d);
		//device_list_.push_back(std::move(OpenCLDevice(*context_, d, *worker_reference)));
	}

	LoadAllBinaries(worker);

	// Mark the manager as having been properly initialized
	valid_ = true;
}

const cl::Context& OpenCLManager::GetContext() const {
	return *context_;
}

std::string OpenCLManager::GetPlatformName() const {
	return platform_name_;
}

std::vector<OpenCLDevice>& OpenCLManager::GetDevices(const Worker& worker) {
	return device_list_;
}

const std::vector<std::shared_ptr<cl::CommandQueue> > OpenCLManager::GetQueues() const {
	return queues_;
}

// May throw
const std::vector<OpenCLKernel> OpenCLManager::CreateKernels(const Worker& worker, const std::vector<std::string>& filenames) {
	std::vector<OpenCLKernel> kernels;

	// Read, compile and link the Program from OpenCL code
	cl::Program::Sources sources = ReadOpenCLSourceCode(filenames);

	for (auto& device : device_list_) {
		device.LoadKernelsFromSource(worker, filenames);
	}

	return kernels;
}

bool OpenCLManager::IsValid() const {
	return valid_;
}


/************************************************************************/
/* Private Functions                                                    */
/************************************************************************/


int OpenCLManager::TrackBuffer(const Worker& worker, GenericBuffer* buffer){
	auto success = false;
	//auto worker = buffer->GetInitialWorker();
	
	//try to retrieve a new buffer ID
	auto buffer_id  = buffer_id_count_++;
	if (buffer_id == invalid_buffer_id_) {
		buffer_id = buffer_id_count_++;
	}

	//TODO: See Dan for how to C++11 mutex good bruh
	if (!buffer_store_.count(buffer_id)){
		buffer_store_.insert({buffer_id, buffer});
		success = true;
	} else {
		LogError(worker, __func__, "Got Duplicate Buffer ID: " + std::to_string(buffer_id));
		buffer_id = invalid_buffer_id_;
	}
	return buffer_id;
}

void OpenCLManager::UntrackBuffer(int buffer_id) {
	buffer_store_.erase(buffer_id);
}

bool OpenCLManager::LoadAllBinaries(const Worker& worker) {

    /*if (load_balancer_ == NULL) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Trying to load binaries when no load balancer is specified");
            return false;
    }*/

    //std::string plat_name = manager_->GetPlatformName();

    bool did_all_succeed = true;
    for (auto& device : device_list_) {
        std::string dev_name = device.GetName();
        std::string binary_path = GetSourcePath("binaries/"+platform_name_+"-"+dev_name+".clbin");
        bool success = false;
		try {
			success = device.LoadKernelsFromBinary(worker, binary_path);
		} catch (const std::exception& e) {
			LogError(worker, __func__,
                "Failed to load binary for device "+dev_name+":\n"+e.what());
            did_all_succeed = false;
		}
        if (!success) {
            LogError(worker, __func__,
                "Failed to load binary for device "+dev_name);
            did_all_succeed = false;
        } else {
			std::cout << "finished reading precompiled binary for " << dev_name << ", list of avaialble kernels: " << std::endl;
			for (auto& kernel : device.GetKernels()) {
				std::cout << " - " << kernel.get().GetName() << std::endl;
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