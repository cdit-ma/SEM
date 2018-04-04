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
		static OpenCLManager* GetReferenceByPlatformID(const Worker& worker, int platform_id);


		static const std::vector<cl::Platform> GetPlatforms(const Worker& worker);


		const cl::Context& GetContext() const;

		std::string GetPlatformName() const;

		std::vector<OpenCLDevice>& GetDevices(const Worker& worker);

		const std::vector<std::shared_ptr<cl::CommandQueue> > GetQueues() const;

		const std::vector<OpenCLKernel> CreateKernels(const Worker& worker, const std::vector<std::string>& filenames);
		
		template <typename T>
		OCLBuffer<T>* CreateBuffer(const Worker& worker, size_t buffer_size);
		template <typename T>
		OCLBuffer<T>* CreateBuffer(const Worker& worker, const std::vector<T>& data, OpenCLDevice& device, bool blocking=true);

		template <typename T>
		void ReleaseBuffer(const Worker& worker, OCLBuffer<T>* buffer);
		
		bool IsValid() const;

		class BufferAttorney {
			BufferAttorney() = delete;
		private:
			friend class GenericBuffer;
			static int GetNewBufferID(const Worker& worker, OpenCLManager& manager, GenericBuffer& buffer) {
				return manager.TrackBuffer(worker, &buffer);
			}
			static void ReleaseBufferID(OpenCLManager& manager, GenericBuffer& buffer) {
				manager.UntrackBuffer(buffer.GetID());
			}
		};

		static const int invalid_buffer_id_ = -1;

	/*protected:
		int GetNewBufferID();*/

	private:
		OpenCLManager(const Worker& worker, cl::Platform &platform);
		~OpenCLManager() {};

		int TrackBuffer(const Worker& worker, GenericBuffer* buffer);
		void UntrackBuffer(int buffer_id);
		void Initialise();

    	bool LoadAllBinaries(const Worker& worker);

		static void LogError(const Worker& worker,
							std::string function_name,
							std::string error_message,
							int cl_error_code);
		static void LogError(const Worker& worker,
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
OCLBuffer<T>* OpenCLManager::CreateBuffer(const Worker& worker, size_t buffer_size){
	//TODO: See Dan for how to mutex good bruh
	auto buffer = new OCLBuffer<T>(worker, *this, /*buffer_id_count_++,*/ buffer_size);
	return buffer;//TrackBuffer<T>(buffer);
}

template <typename T>
OCLBuffer<T>* OpenCLManager::CreateBuffer(const Worker& worker, const std::vector<T>& data, OpenCLDevice& device, bool blocking) {
	//TODO: See Dan for how to mutex good bruh
	auto buffer = new OCLBuffer<T>(worker, *this, data, device, blocking);
	return buffer;
}


template <typename T>
void OpenCLManager::ReleaseBuffer(const Worker& worker, OCLBuffer<T>* buffer) {
	delete buffer;
}

#endif // OPENCL_MANAGER_H