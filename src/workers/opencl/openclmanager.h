#ifndef OPENCL_MANAGER_H
#define OPENCL_MANAGER_H


#include <unordered_map>
#include <map>
#include <mutex>

#include <core/modellogger.h>
#include <core/worker.h>

#include "oclbuffer.hpp"
#include "opencldevice.h"

class OpenCLKernel;

namespace cl {
	class Platform;
	class Context;
	class CommandQueue;
	class Program;
	class Kernel;
}

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

		std::string GetPlatformName() const;

		std::vector<OpenCLDevice>& GetDevices(Worker* worker_reference=NULL);

		const std::vector<std::shared_ptr<cl::CommandQueue> > GetQueues() const;

		const std::vector<OpenCLKernel> CreateKernels(const std::vector<std::string>& filenames, Worker* worker_reference = NULL);
		
		template <typename T>
		OCLBuffer<T>* CreateBuffer(size_t buffer_size, Worker* worker_reference=NULL);
		template <typename T>
		OCLBuffer<T>* CreateBuffer(const std::vector<T>& data, OpenCLDevice& device,  bool blocking=true, Worker* worker_reference=NULL);

		template <typename T>
		void ReleaseBuffer(OCLBuffer<T>* buffer, Worker* worker_reference=NULL);
		
		bool IsValid() const;

		class BufferAttorney {
			BufferAttorney() = delete;
		private:
			friend class GenericBuffer;
			static int GetNewBufferID(OpenCLManager& manager, GenericBuffer& buffer) {
				return manager.TrackBuffer(&buffer);
			}
			static void ReleaseBufferID(OpenCLManager& manager, GenericBuffer& buffer) {
				manager.UntrackBuffer(buffer.GetID());
			}
		};

		static const int invalid_buffer_id_ = -1;

	/*protected:
		int GetNewBufferID();*/

	private:
		OpenCLManager(cl::Platform &platform, Worker* worker_reference=NULL);
		~OpenCLManager() {};

		int TrackBuffer(GenericBuffer* buffer);
		void UntrackBuffer(int buffer_id);
		void Initialise();

    	bool LoadAllBinaries(Worker* worker_ref);

		static void LogError(Worker* worker_reference,
							std::string function_name,
							std::string error_message,
							int cl_error_code);
		static void LogError(Worker* worker_reference,
							std::string function_name,
							std::string error_message);

		// A separate OpenCLManager is maintained for each OpenCL platform
		static std::vector<OpenCLManager*> reference_list_;
		static std::vector<cl::Platform> platform_list_;

		// Variables
		bool valid_ = false;
		cl::Platform& platform_;
		std::string platform_name_;
		cl::Context* context_;
		std::vector<OpenCLDevice> device_list_;
		std::vector<std::shared_ptr<cl::CommandQueue> > queues_;
		cl::Program* program_;
		std::vector< std::vector<cl::Kernel>* >  kernel_vector_store_;

		std::map<int, GenericBuffer*> buffer_store_;
		int buffer_id_count_ = -1;
};


template <typename T>
OCLBuffer<T>* OpenCLManager::CreateBuffer(size_t buffer_size, Worker* worker_reference){
	//TODO: See Dan for how to mutex good bruh
	auto buffer = new OCLBuffer<T>(*this, /*buffer_id_count_++,*/ buffer_size, worker_reference);
	return buffer;//TrackBuffer<T>(buffer);
}

template <typename T>
OCLBuffer<T>* OpenCLManager::CreateBuffer(const std::vector<T>& data, OpenCLDevice& device, bool blocking, Worker* worker_reference) {
	//TODO: See Dan for how to mutex good bruh
	auto buffer = new OCLBuffer<T>(*this, data, device, blocking, worker_reference);
	return buffer;
}


template <typename T>
void OpenCLManager::ReleaseBuffer(OCLBuffer<T>* buffer,
		Worker* worker_reference) {

	delete buffer;
}

#endif // OPENCL_MANAGER_H
