// WE_GPU.mpc  2015-08-17  Jackson Michael

#ifndef WORKERS_GPU_GPUWORKER_IMPL_H
#define WORKERS_GPU_GPUWORKER_IMPL_H

#include "cl.hpp"
#include "BufferHashtable.h"
#include <string>
#include <clFFT.h>
#include <mutex>

class Gpu_Worker_Impl {
public:
	Gpu_Worker_Impl();
	~Gpu_Worker_Impl();

	// Set up the OpenCL environment as well as creating kernels and buffers neccessary for
	// running kernels of arbitrary workitem sizes at a later point
	void Initialise(bool forceGPU=true);
	
	// Clean up all openCL related data, including any manually allocated buffers that havent
	// been explicitly deallocated
	void Release();

	// Returns whether or not the worker can currently perform meaningful GPU work
	bool IsValid();
	
	// Returns the first found valid openCL platforms name
	std::string PlatformName();

	// Returns the name the device reports to openCL
	unsigned int NumDevices();

	// Returns the name the device reports to openCL, or "INVALID WORKER"/"INVALID DEVICE" 
	std::string DeviceName(unsigned int gpuNum=0);

	// Returns the amount of memory available in bytes for the indicated device
	size_t MemCapacity(unsigned int gpuNum=0);

	// Buffer a given amount of data (in kilobytes), optionally copying that across to the GPU by default
	bool BufferData(size_t bytes, bool forceCopy=true, bool blocking=false, unsigned int gpuNum=0);

	// Release a given amount of buffers
	bool ReleaseData(size_t bytes, bool forceCopy, bool blocking=true, unsigned int gpuNum=0);

	// Run a highly parallel task using pre-buffered data
	void RunParallel(unsigned int numThreads, unsigned int opsPerThread, unsigned int gpuNum=0);

	// Run several FFTs using totalBytes of input data, with each FFT processing sampleLength of data.
	// sampleLength should be a power of 2, and will be converted 
	void PerformFFT_SP(void* dataIn, size_t dataBytes, unsigned int gpuNum=0);
	void PerformFFT_SP(size_t dataBytes, unsigned int gpuNum=0);

	// Multiply two matrices of size n x n
	void MatrixMult(unsigned int n, unsigned int gpuNum=0);
	// Multiply two matrices using memory from dataA and dataB
	// with the product placed into dataC, returns whether or not operation suceeded
	bool MatrixMult(unsigned int widthA, unsigned int heightA, unsigned int widthB,
					const void* dataA, const void* dataB, void* dataOut,
					int gpuNum=0);
	struct PlatformInfo{
		int platform_number;
		int device_number;
		std::string platform_name;
		std::string device_name;
	};

	std::vector<PlatformInfo> GetPlatformInfo();

private:


	bool valid;

	std::vector<PlatformInfo> platform_info_;

	// OpenCL data structures
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	std::string platform_name_;

	cl::Context* context;
	std::vector<cl::CommandQueue*> queues;
	cl::Program* program;
	cl::Kernel* parallelKernel;
	cl::Kernel* partiallyParallelKernel;
	cl::Kernel* matrixKernel;

	//matrix lock
	std::mutex matrixLock_;

	// Blank buffers for allowing kernels to work on data
	cl::Buffer* blankBuffer;
	cl::Buffer* blankBuffer2;
	cl::Buffer* blankBuffer3;

	cl_uint blankdatasize;
	cl_uint numCentroids;

	// Hashtable for recording which buffers have been allocated
	BufferHashtable* hashtable;

	// Things should not be sharing this!!!!!
	// Make a list that deletes the relevant entry when the event's callback is triggered instead!
	cl::Event readCopyFinished;

	// FFT plan handle
    clfftSetupData fftSetup;

	void InitCLFFT();
	void CleanupCLFFT();
};

#endif //WORKERS_GPU_GPUWORKER_IMPL_H