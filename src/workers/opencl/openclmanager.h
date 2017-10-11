#ifndef OPENCL_MANAGER_H
#define OPENCL_MANAGER_H


#include <unordered_map>
#include <map>
#include <mutex>

#include <core/modellogger.h>
#include <core/worker.h>

#include "oclbuffer.hpp"
#include "openclkernel.hpp"

class OpenCLManager {
	public:
		/**
		* Returns the OpenCLManager responsible for managing a given OpenCL platform,
		* constructing and initializing the appropriate resources if required.
		*
		* @param platformID the index of the OpenCL platform as specified by the deployment information
		* @param workerReference a reference to the worker making the call (for logging)
		* @return The OpenCLMaanager for the provided platform, or NULL if one can't be created
		**/
		static OpenCLManager* GetReferenceByPlatform(int platform_id, Worker* worker_reference=NULL);


		static const std::vector<cl::Platform> GetPlatforms(Worker* worker_reference=NULL);


		const cl::Context& GetContext() const;

		const std::vector<cl::Device> GetDevices(Worker* worker_reference=NULL) const;

		const std::vector<cl::CommandQueue> GetQueues() const;

		const std::vector<OpenCLKernel> CreateKernels(std::vector<std::string> filenames, Worker* worker_reference = NULL);

		template <typename KernelArg_t>
		void SetKernelArg(cl::Kernel& kernel, cl_int index, KernelArg_t value);

		template <typename KernelArg_t>
		void SetKernelArg(cl::Kernel& kernel, cl_int index, OCLBuffer<KernelArg_t> value);
		
		template <typename T>
		OCLBuffer<T>* CreateBuffer(size_t buffer_size, Worker* worker_reference=NULL);
		template <typename T>
		OCLBuffer<T>* CreateBuffer(const std::vector<T>& data, Worker* worker_reference=NULL);

		template <typename T>
		void ReleaseBuffer(OCLBuffer<T>* buffer, Worker* worker_reference=NULL);
		
		bool IsValid() const;

		class BufferAttorney {
			BufferAttorney() = delete;
		private:
			friend class GenericBuffer;
			static int GetNewBufferID(OpenCLManager& manager) {
				return manager.buffer_id_count_++;
			}
		};

	/*protected:
		int GetNewBufferID();*/

	private:
		OpenCLManager(cl::Platform &platform, Worker* worker_reference=NULL);
		~OpenCLManager() {};

		template <typename T>
		OCLBuffer<T>* TrackBuffer(OCLBuffer<T>* buffer);
		void UntrackBuffer(int buffer_id);
		void Initialise();

		cl::Program::Sources ReadOpenCLSourceCode(const std::vector<std::string>& filenames,
			Worker* worker_reference=NULL);
		
		
		template <typename T>
		friend OCLBuffer<T>::~OCLBuffer();
		

		static void LogError(Worker* worker_reference,
							std::string function_name,
							std::string error_message,
							cl_int cl_error_code);
		static void LogError(Worker* worker_reference,
							std::string function_name,
							std::string error_message);

		// A separate OpenCLManager is maintained for each OpenCL platform
		static std::vector<OpenCLManager*> reference_list_;
		static std::vector<cl::Platform> platform_list_;

		// Variables
		bool valid_ = false;
		cl::Platform& platform_;
		cl::Context* context_;
		std::vector<cl::Device> device_list_;
		std::vector<cl::CommandQueue> queues_;
		cl::Program* program_;
		std::vector< std::vector<cl::Kernel>* >  kernel_vector_store_;

		std::map<int, GenericBuffer*> buffer_store_;
		int buffer_id_count_ = -1;
};


template <typename T>
OCLBuffer<T>* OpenCLManager::TrackBuffer(OCLBuffer<T>* buffer){
	auto success = false;
	auto worker = buffer->GetWorkerReference();
	
	if(buffer->is_valid()) {
		auto buffer_id = buffer->GetID();

		//TODO: See Dan for how to C++11 mutex good bruh
		if (!buffer_store_.count(buffer_id)){
			buffer_store_.insert({buffer_id, buffer});
			success = true;
		} else {
			LogError(worker, __func__, "Got Duplicate Buffer ID: " + std::to_string(buffer_id));
		}
	} else {
		LogError(worker, __func__, "Failed to create a valid buffer");
	}

	if (!success) {
		//If we have an error, teardown the buffer if it's in memory
		delete buffer;
		buffer = 0;
	}
	return buffer;
}

template <typename T>
OCLBuffer<T>* OpenCLManager::CreateBuffer(size_t buffer_size, Worker* worker_reference){
	//TODO: See Dan for how to mutex good bruh
	auto buffer = new OCLBuffer<T>(this, /*buffer_id_count_++,*/ buffer_size, worker_reference);
	return buffer;//TrackBuffer<T>(buffer);
}

template <typename T>
OCLBuffer<T>* OpenCLManager::CreateBuffer(const std::vector<T>& data, Worker* worker_reference) {
	//TODO: See Dan for how to mutex good bruh
	auto buffer = new OCLBuffer<T>(this, /*buffer_id_count_++,*/ data, worker_reference);
	return buffer;//TrackBuffer<T>(buffer);
}


template <typename T>
void OpenCLManager::ReleaseBuffer(OCLBuffer<T>* buffer,
		Worker* worker_reference) {

	delete buffer;
	/*if (buffer->is_valid()) {
		buffer_store_.erase(buffer->GetID());
	}*/
}

#endif // OPENCL_MANAGER_H
