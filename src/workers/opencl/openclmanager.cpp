
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


OpenCLManager* OpenCLManager::GetReferenceByPlatform(int platformID, Worker* workerReference) {
	// If we haven't initialized the length of the reference list yet do so now
	if (reference_list_.empty()) {
		cl_int errCode = cl::Platform::get(&platform_list_);
		if (errCode != CL_SUCCESS) {
			LogError(workerReference,
					std::string(__func__),
					"Failed to retrieve the list of OpenCL platforms",
					errCode);
			return NULL;
		}

		reference_list_.resize(platform_list_.size(), NULL);
	}

	// Check that the specified platform index isn't out of bounds
	if (platformID >= reference_list_.size()) {
		LogError(workerReference,
				std::string(__func__),
				"PlatformID is out of bounds (" + std::to_string(platformID) + ")");
		return NULL;
	}
	
	// If we haven't created an OpenCLManager for the specified platform do so now
	if (reference_list_[platformID] == NULL) {
		OpenCLManager* newManager = new OpenCLManager(platform_list_[platformID]);

		if (!newManager->IsValid()) {
			LogError(workerReference,
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

const std::vector<cl::Platform> OpenCLManager::GetPlatforms(Worker* worker_reference) {
	cl_int err_code = cl::Platform::get(&platform_list_);
	if (err_code != CL_SUCCESS) {
		LogError(worker_reference,
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


OpenCLManager::OpenCLManager(cl::Platform& platform_, Worker* worker_reference) : platform_(platform_) {
	cl_int err;

	// Manager shouldn't be marked as valid until creation has completed successfully
	valid_ = false;

	std::string platform_name;
	platform_name = platform_.getInfo<CL_PLATFORM_NAME>(&err);
	if (err != CL_SUCCESS) {
		LogError(worker_reference,
			std::string(__func__),
			"Unable to retrieve platform name",
			err);

	}

	// Pull out all OpenCL devices in the platform (don't distinguish between CPU, GPU, etc)
	std::vector<cl::Device> devices;
	err = platform_.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	if (err != CL_SUCCESS) {
		std::string platform_name = platform_.getInfo<CL_PLATFORM_NAME>();
		LogError(worker_reference,
			std::string(__func__),
			"Failed to retrieve the list of devices for platform " + platform_name,
			err);
	}

	if (devices.size() == 0)
	{
		LogError(worker_reference,
			std::string(__func__),
			"Unable to find any devices for the given OpenCL platform (" + platform_name + ")");
		return;
	}

	// Create a context from the collected devices
	context_ = new cl::Context(devices, NULL, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		LogError(worker_reference,
			std::string(__func__),
			"Unable to create an OpenCL context (" + platform_name + ")",
			err);
		return;
	}

	// Create a representation for each of the devices
	for(auto& d : devices) {
		device_list_.emplace_back(*this, d, *worker_reference);
		//device_list_.push_back(std::move(OpenCLDevice(*context_, d, *worker_reference)));
	}

	// Mark the manager as having been properly initialized
	valid_ = true;
}

const cl::Context& OpenCLManager::GetContext() const {
	return *context_;
}



std::vector<OpenCLDevice>& OpenCLManager::GetDevices(Worker* worker_reference) {
	return device_list_;
}

const std::vector<std::shared_ptr<cl::CommandQueue> > OpenCLManager::GetQueues() const {
	return queues_;
}

const std::vector<OpenCLKernel> OpenCLManager::CreateKernels(const std::vector<std::string>& filenames, Worker* worker_reference) {
	std::vector<OpenCLKernel> kernels;

	// Read, compile and link the Program from OpenCL code
	cl::Program::Sources sources = ReadOpenCLSourceCode(filenames, worker_reference);

	for (auto& device : device_list_) {
		device.LoadKernelsFromSource(filenames, *worker_reference);
	}

	return kernels;
}

bool OpenCLManager::IsValid() const {
	return valid_;
}


/************************************************************************/
/* Private Functions                                                    */
/************************************************************************/

int OpenCLManager::TrackBuffer(GenericBuffer* buffer){
	auto success = false;
	auto worker = buffer->GetInitialWorker();
	
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

	/*if (!success) {
		//If we have an error, teardown the buffer if it's in memory
		delete buffer;
		buffer = 0;
	}*/
	return buffer_id;
}

void OpenCLManager::UntrackBuffer(int buffer_id) {
	buffer_store_.erase(buffer_id);
}

void OpenCLManager::LogError(Worker* worker_reference,
							std::string function_name,
							std::string error_message,
							int cl_error_code)
{
	
	LogOpenCLError(worker_reference,
		"OpenCLManager::" + function_name,
		error_message,
		cl_error_code);
}

void OpenCLManager::LogError(Worker* worker_reference,
							std::string function_name,
							std::string error_message)
{
	LogOpenCLError(worker_reference,
		"OpenCLManager::" + function_name,
		error_message);
}