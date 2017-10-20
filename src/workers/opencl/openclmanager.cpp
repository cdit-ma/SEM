
#include "openclutilities.h"
#include "openclmanager.h"
#include "openclkernel.hpp"

#include <core/worker.h>

#include <string>
#include <fstream>
#include <sstream>

#include <unordered_set>

std::vector<OpenCLManager*> OpenCLManager::reference_list_;
std::vector<cl::Platform> OpenCLManager::platform_list_;



cl::Program::Sources ReadOpenCLSourceCode(const std::vector<std::string>& filenames,
	Worker* worker_reference=NULL);


std::vector<cl::Device> Device2Value(const std::vector<std::shared_ptr<cl::Device> > device_list ){
	std::vector<cl::Device> devices;
	for (auto d : device_list) {
		devices.push_back(*d);
	}
	return devices;
}

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

	}\
	device_list_ = GetDevices(worker_reference);
	if (device_list_.size() == 0)
	{
		LogError(worker_reference,
			std::string(__func__),
			"Unable to find any devices for the given OpenCL platform (" + platform_name + ")");
		return;
	}

	auto devices = Device2Value(device_list_);
	context_ = new cl::Context(devices, NULL, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		LogError(worker_reference,
			std::string(__func__),
			"Unable to create an OpenCL context (" + platform_name + ")",
			err);
		return;
	}

	for (auto d : devices) {

		queues_.push_back(std::make_shared<cl::CommandQueue>(*context_, d, NULL, &err));
		if (err != CL_SUCCESS) {
			LogError(worker_reference,
				std::string(__func__),
				"Unable to create a command queue for device " + d.getInfo<CL_DEVICE_NAME>(),
				err);
			return;
		}
	}

	// Mark the manager as having been properly initialized
	valid_ = true;
}

const cl::Context& OpenCLManager::GetContext() const {
	return *context_;
}



const std::vector<std::shared_ptr<cl::Device> > OpenCLManager::GetDevices(Worker* worker_reference) const {
	cl_int err;
	std::vector<cl::Device> devices;

	// Pull out all devices (don't distinguish between CPU, GPU, etc)
	err = platform_.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	if (err != CL_SUCCESS) {
		std::string platform_name = platform_.getInfo<CL_PLATFORM_NAME>();
		LogError(worker_reference,
			std::string(__func__),
			"Failed to retrieve the list of devices for platform " + platform_name,
			err);
	}
	std::vector<std::shared_ptr<cl::Device> > shared_devices;
	for(auto d : devices) {
		shared_devices.emplace_back(std::shared_ptr<cl::Device>(new cl::Device(d)));
	}

	return shared_devices;
}

const std::vector<std::shared_ptr<cl::CommandQueue> > OpenCLManager::GetQueues() const {
	return queues_;
}

const std::vector<OpenCLKernel> OpenCLManager::CreateKernels(std::vector<std::string> filenames, Worker* worker_reference) {
	cl_int err;

	std::vector<OpenCLKernel> kernels;

	// Read, compile and link the Program from OpenCL code
	cl::Program::Sources sources = ReadOpenCLSourceCode(filenames, worker_reference);

	program_ = new cl::Program(*context_, sources, &err);
	if (err != CL_SUCCESS) {
		LogError(worker_reference,
			std::string(__func__),
			"Unable to create OpenCL program from OpenCL source code",
			err);
		return kernels;
	}
	const auto devices = Device2Value(device_list_);
	err = program_->build(devices);
	if (err != CL_SUCCESS) {
		LogError(worker_reference,
			std::string(__func__),
			"An error occurred while building an OpenCL program: \n" +
			program_->getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]),
			err);
		return kernels;
	}

	std::vector<cl::Kernel>* cl_kernels = new std::vector<cl::Kernel>();
	kernel_vector_store_.push_back(cl_kernels);
	err = program_->createKernels(cl_kernels);
	if (err != CL_SUCCESS) {
		LogError(worker_reference,
			std::string(__func__),
			"An error occurred during the creation of OpenCL kernels from a built program",
			err);
		return kernels;
	}

	for (auto &cl_kernel : *cl_kernels) {
		// kernels.push_back(new OpenCLKernel(*this, cl_kernel, worker_reference));
		kernels.emplace_back(*this, cl_kernel, worker_reference);
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

/**
 * Takes a list of filenames and returns a list of c-strings containg their contents
 * Note that the c-strings will need to be freed by the caller!
 **/
cl::Program::Sources ReadOpenCLSourceCode(
											const std::vector<std::string>& filenames,
											Worker* worker_reference/*=NULL*/) {
	cl::Program::Sources source_list;

	// For now, just add the OpenCL file in the source directory
	/*std::vector<std::string> filenames = { source_dir};
	filenames[0] = filenames[0].substr(0, filenames[0].find_last_of("/\\"));
	filenames[0].append("/kernel.cl");*/

	for (const auto& filename : filenames) {
		std::ifstream source_file;
		source_file.open(filename);
		if (!source_file.is_open()) {
			LogOpenCLError(worker_reference,
				__func__,
				"Failed to open file when reading source files: " + filename);
			break;
		}

		std::stringstream source_stream;
		source_stream << source_file.rdbuf();

		//size_t str_len = source_stream.str().size();
		/*char* source_string = new char[str_len];
		//strncpy(source_string, source_stream.str().c_str(), str_len);
		size_t len = source_stream.str().copy(source_string, str_len, 0);
		source_string[len] = '\0';

		auto new_source = std::pair<const char*, size_t>(source_string, str_len);
		source_list.push_back(new_source);*/
		
		source_list.push_back(source_stream.str());
	}
	

	return source_list;
}

//void OpenCLManager::DereferenceBuffer(int id) {
	//buffer_store_.erase(id);
//}

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